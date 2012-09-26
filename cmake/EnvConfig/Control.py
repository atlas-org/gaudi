'''
Created on Jun 27, 2011

@author: mplajner
'''
import xmlModule
import os
from time import gmtime, strftime
import Variable

class Environment():
    '''object to hold settings of environment'''

    def __init__(self, loadFromSystem=True, useAsWriter=False, reportLevel=1, searchPath=None):
        '''Initial variables to be pushed and setup

        append switch between append and prepend for initial variables.
        loadFromSystem causes variable`s system value to be loaded on first encounter.
        If useAsWriter == True than every change to variables is recorded to XML file.
        reportLevel sets the level of messaging.
        '''
        self.report = xmlModule.Report(reportLevel)

        self.separator = ':'

        # Prepeare the internal search path for xml files (used by 'include' elements)
        self.searchPath = ['.']
        if searchPath is not None:
            self.searchPath.extend(searchPath)
        try:
            self.searchPath.extend(os.environ['ENVXMLPATH'].split(os.pathsep))
        except KeyError:
            # ignore if the env variable is not there
            pass

        self.actions = {}
        self.actions['include'] = lambda n, c, _2: self.loadXML(self._locate(n, c))
        self.actions['append'] = lambda n, v, _: self.append(n, v)
        self.actions['prepend'] = lambda n, v, _: self.prepend(n, v)
        self.actions['set'] = lambda n, v, _: self.set(n, v)
        self.actions['unset'] = lambda n, v, _: self.unset(n, v)
        self.actions['remove'] = lambda n, v, _: self.remove(n, v)
        self.actions['remove-regexp'] = lambda n, v, _: self.remove_regexp(n, v)
        self.actions['declare'] = self.declare

        self.variables = {}
        self.loadFromSystem = loadFromSystem
        self.asWriter = useAsWriter
        if useAsWriter:
            self.writer = xmlModule.XMLFile()
            self.startXMLinput()

    def _locate(self, filename, caller=None):
        '''
        Find 'filename' in the internal search path.
        '''
        from os.path import isabs, isfile, join, dirname, normpath
        if isabs(filename):
            return filename
        if caller:
            localfile = join(dirname(caller), filename)
            if isfile(localfile):
                return localfile
        try:
            return (f for f in [normpath(join(d, filename))
                                for d in self.searchPath]
                    if isfile(f)).next()
        except StopIteration:
            from errno import ENOENT
            raise OSError(ENOENT, 'cannot find file in %r' % self.searchPath, filename)

    def vars(self, strings=True):
        '''returns dictionary of all variables optionally converted to string'''
        if strings:
            variables = self.variables.copy()
            for item in variables:
                variables[item] = self.var(item).value(True)
            return variables
        else:
            return self.variables


    def var(self, name):
        '''Gets a single variable. If not available then tries to load from system.'''
        if name in self.variables.keys():
            return self.variables[name]
        else:
            return os.environ[name]


    def search(self, varName, expr, regExp = False):
        '''Searches in a variable for a value.'''
        return self.variables[varName].search(expr, regExp)


    def include(self, name, value):
        '''Appends to an existing variable.'''
        if self.asWriter:
            self._writeVarToXML(name, 'append', value)
        else:
            if name in self.variables.keys():
                self.variables[name].append(value, self.separator, self.variables)
            else:
                self.declare(name, 'list', False)
                self.append(name, value)

    def append(self, name, value):
        '''Appends to an existing variable.'''
        if self.asWriter:
            self._writeVarToXML(name, 'append', value)
        else:
            if name in self.variables.keys():
                self.variables[name].append(value, self.separator, self.variables)
            else:
                self.declare(name, 'list', False)
                self.append(name, value)


    def prepend(self, name, value):
        '''Prepends to an existing variable, or create a new one.'''
        if self.asWriter:
            self._writeVarToXML(name, 'prepend', value)
        else:
            if name in self.variables.keys():
                self.variables[name].prepend(value, self.separator, self.variables)
            else:
                self.declare(name, 'list', False)
                self.prepend(name, value)

    def declare(self, name, vartype, local):
        '''Creates an instance of new variable. It loads values from the OS if the variable is not local.'''
        if self.asWriter:
            self._writeVarToXML(name, 'declare', '', vartype, local)

        if not isinstance(local, bool):
            if str(local).lower() == 'true':
                local = True
            else:
                local = False

        if name in self.variables.keys():
            if self.variables[name].local != local:
                raise Variable.EnvError(name, 'redeclaration')
            else:
                if vartype.lower() == "list":
                    if not isinstance(self.variables[name],Variable.List):
                        raise Variable.EnvError(name, 'redeclaration')
                else:
                    if not isinstance(self.variables[name],Variable.Scalar):
                        raise Variable.EnvError(name, 'redeclaration')

        if vartype.lower() == "list":
            a = Variable.List(name, local, report=self.report)
        else:
            a = Variable.Scalar(name, local, report=self.report)

        if self.loadFromSystem and not local:
            if name in os.environ.keys():
                a.set(os.environ[name], os.pathsep, environment=self.variables, resolve=False)
        self.variables[name] = a

    def set(self, name, value):
        '''Sets a single variable - overrides any previous value!'''
        name = str(name)
        if self.asWriter:
            self._writeVarToXML(name, 'set', value)
        else:
            if name in self.variables:
                self.variables[name].set(value, self.separator, self.variables)
            else:
                self.declare(name, 'list', False)
                self.set(name, value)

    def unset(self, name, value=None):# pylint: disable=W0613
        '''Unsets a single variable to an empty value - overrides any previous value!'''
        if self.asWriter:
            self._writeVarToXML(name, 'unset', '')
        else:
            if name in self.variables:
                self.variables[name].set([], self.separator)
            else:
                a = Variable.List(name, report=self.report)
                self.variables[name] = a


    def remove(self, name, value, regexp=False):
        '''Remove value from variable.'''
        if self.asWriter:
            self._writeVarToXML(name, 'remove', value)
        else:
            if name in self.variables:
                self.variables[name].remove(value, self.separator, regexp)
            elif self.loadFromSystem:
                self.declare(name, 'list', False)


    def remove_regexp(self, name, value):
        self.remove(name, value, True)


    def searchFile(self, filename, varName):
        '''Searches for appearance of variable in a file.'''
        XMLFile = xmlModule.XMLFile()
        variable = XMLFile.variable(filename, name=varName)
        return variable

    def loadXML(self, fileName=None, namespace='EnvSchema'):
        '''Loads XML file for input variables.'''
        XMLfile = xmlModule.XMLFile()
        fileName = self._locate(fileName)
        variables = XMLfile.variable(fileName, namespace=namespace)
        for i, (action, args) in enumerate(variables):
            if action not in self.actions:
                self.report.addError('Node {0}: No action taken with var "{1}". Probably wrong action argument: "{2}".'.format(i, args[0], action))
            else:
                if action == "include":
                    args = (args[0], fileName, None)
                self.actions[action](*args) # pylint: disable=W0142

    def startXMLinput(self):
        '''Renew writer for new input.'''
        self.writer.resetWriter()


    def finishXMLinput(self, outputFile = ''):
        '''Finishes input of XML file and closes the file.'''
        self.writer.writeToFile(outputFile)


    def writeToFile(self, fileName, shell='sh'):
        '''Creates an output file with a specified name to be used for setting variables by sourcing this file'''
        f = open(fileName, 'w')
        if shell == 'sh':
            f.write('#!/bin/bash\n')
            for variable in self.variables:
                if not self[variable].local:
                    f.write('export ' +variable+'='+self[variable].value(True, os.pathsep)+'\n')
        elif shell == 'csh':
            f.write('#!/bin/csh\n')
            for variable in self.variables:
                if not self[variable].local:
                    f.write('setenv ' +variable+' '+self[variable].value(True, os.pathsep)+'\n')
        else:
            f.write('')
            f.write('REM This is an enviroment settings file generated on '+strftime("%a, %d %b %Y %H:%M:%S\n", gmtime()))
            for variable in self.variables:
                if not self[variable].local:
                    f.write('set '+variable+'='+self[variable].value(True, os.pathsep)+'\n')

        f.close()


    def writeToXMLFile(self, fileName):
        '''Writes the current state of environment to a XML file.

        NOTE: There is no trace of actions taken, variables are written with a set action only.
        '''
        writer = xmlModule.XMLFile()
        for var in self.variables.keys():
            writer.writeVar(var, 'set', self.variables[var].value(True, self.separator))
        writer.writeToFile(fileName)


    def loadAllSystemVariables(self):
        '''Loads all variables from the current system settings.'''
        for val in os.environ.keys():
            if not val in self.variables.keys():
                self.declare(val, 'list', False)


    def _concatenate(self, value):
        '''Returns a variable string with separator separator from the values list'''
        stri = ""
        for it in value:
            stri += it + self.separator
        stri = stri[0:len(stri)-1]
        return stri


    def _writeVarToXML(self, name, action, value, vartype='list', local='false'):
        '''Writes single variable to XML file.'''
        if isinstance(value, list):
            value = self._concatenate(value)
        self.writer.writeVar(name, action, value, vartype, local)


    def __getitem__(self, key):
        return self.variables[key]

    def __setitem__(self, key, value):
        if key in self.variables.keys():
            self.report.addWarn('Addition canceled because of duplicate entry. Var: "' + key + '" value: "' + value + '".')
        else:
            self.append(key, value)

    def __delitem__(self, key):
        del self.variables[key]

    def __iter__(self):
        for i in self.variables:
            yield i

    def __contains__(self, item):
        return item in self.variables.keys()

    def __len__(self):
        return len(self.variables.keys())

