########################################################################
# File:   CoolTestExtensions.py
# Author: Marco Clemencic CERN/PH-LBC
# Date:   16/10/2007  
#
# Contents:
#   TemporaryEnvironment: class to change the environment and revert to
#                         the original one
#
#   DBPreparer: handle the details for temporary test databases
#
#   StandardTest: extension to ShellCommandTest that finds Python
#                 scripts in the PATH ad run them through the
#                 interpreter
#
#   DatabaseTest: run a test that needs a database
#
#   SourceTargetTest: run a test that needs 2 databases
#                     (source and target)
#
# Copyright (c) 2003 by CodeSourcery, LLC.  All rights reserved.
#
########################################################################
__author__  = 'Marco Clemencic CERN/PH-LBC' 
__version__ = "$Revision: 1.31 $" 
__tag__     = "$Name:  $" 
########################################################################
# Imports
########################################################################
import os
import sys
import re
import tempfile
import shutil
import string
import difflib
from subprocess import Popen, PIPE, STDOUT

import qm
from qm.test.classes.command import ExecTestBase

########################################################################
# Utility Classes
########################################################################
class TemporaryEnvironment:
    """
    Class to changes the environment temporarily.
    """
    def __init__(self, orig = os.environ, keep_same = False):
        """
        Create a temporary environment on top of the one specified
        (it can be another TemporaryEnvironment instance).
        """
        #print "New environment"
        self.old_values = {}
        self.env = orig
        self._keep_same = keep_same
        
    def __setitem__(self,key,value):
        """
        Set an environment variable recording the previous value.
        """
        if key not in self.old_values :
            if key in self.env :
                if not self._keep_same or self.env[key] != value:
                    self.old_values[key] = self.env[key]
            else:
                self.old_values[key] = None
        self.env[key] = value
        
    def __getitem__(self,key):
        """
        Get an environment variable.
        Needed to provide the same interface as os.environ.
        """
        return self.env[key]
    
    def __delitem__(self,key):
        """
        Unset an environment variable.
        Needed to provide the same interface as os.environ.
        """
        if key not in self.env :
            raise KeyError(key)
        self.old_values[key] = self.env[key]
        del self.env[key]

    def keys(self):
        """
        Return the list of defined environment variables.
        Needed to provide the same interface as os.environ.
        """
        return self.env.keys()
        
    def items(self):
        """
        Return the list of (name,value) pairs for the defined environment variables.
        Needed to provide the same interface as os.environ.
        """
        return self.env.items()
    
    def __contains__(self,key):
        """
        Operator 'in'.
        Needed to provide the same interface as os.environ.
        """
        return key in self.env
    
    def restore(self):
        """
        Revert all the changes done to the orignal environment.
        """
        for key,value in self.old_values.items():
            if value is None:
                del self.env[key]
            else:
                self.env[key] = value
        self.old_values = {}
        
    def __del__(self):
        """
        Revert the changes on destruction.
        """
        #print "Restoring the environment"
        self.restore()

    def gen_script(self,shell_type):
        """
        Generate a shell script to reproduce the changes in the environment.
        """
        shells = [ 'csh', 'sh', 'bat' ]
        if shell_type not in shells:
            raise RuntimeError("Shell type '%s' unknown. Available: %s"%(shell_type,shells))
        out = ""
        for key,value in self.old_values.items():
            if key not in self.env:
                # unset variable
                if shell_type == 'csh':
                    out += 'unsetenv %s\n'%key
                elif shell_type == 'sh':
                    out += 'unset %s\n'%key
                elif shell_type == 'bat':
                    out += 'set %s=\n'%key
            else:
                # set variable
                if shell_type == 'csh':
                    out += 'setenv %s "%s"\n'%(key,self.env[key])
                elif shell_type == 'sh':
                    out += 'export %s="%s"\n'%(key,self.env[key])
                elif shell_type == 'bat':
                    out += 'set %s=%s\n'%(key,self.env[key])
        return out

class TempDir:
    """Small class for temporary directories.
    When instantiated, it creates a temporary directory and the instance
    behaves as the string containing the directory name.
    When the instance goes out of scope, it removes all the content of
    the temporary directory (automatic clean-up).
    """
    def __init__(self, keep = False, chdir = False):
        self.name = tempfile.mkdtemp()
        self._keep = keep
        self._origdir = None
        if chdir:
            self._origdir = os.getcwd()
            os.chdir(self.name)
        
    def __str__(self):
        return self.name
    
    def __del__(self):
        if self._origdir:
            os.chdir(self._origdir)
        if self.name and not self._keep:
            shutil.rmtree(self.name)
    
    def __getattr__(self,attr):
        return getattr(self.name,attr)

class TempFile:
    """Small class for temporary files.
    When instantiated, it creates a temporary directory and the instance
    behaves as the string containing the directory name.
    When the instance goes out of scope, it removes all the content of
    the temporary directory (automatic clean-up).
    """
    def __init__(self, suffix='', prefix='tmp', dir=None, text=False, keep = False):
        self.file = None
        self.name = None
        self._keep = keep
        
        self._fd, self.name = tempfile.mkstemp(suffix,prefix,dir,text)
        self.file = os.fdopen(self._fd,"r+")
        
    def __str__(self):
        return self.name
    
    def __del__(self):
        if self.file:
            self.file.close()
        if self.name and not self._keep:
            os.remove(self.name)
    
    def __getattr__(self,attr):
        return getattr(self.file,attr)

class CMT:
    """Small wrapper to call CMT.
    """
    def __init__(self,path=None):
        if path is None:
            path = os.getcwd()
        self.path = path
    
    def _run_cmt(self,command,args):
        # prepare command line
        if type(args) is str:
            args = [args]
        cmd = "cmt %s"%command
        for arg in args:
            cmd += ' "%s"'%arg
        
        # go to the execution directory
        olddir = os.getcwd()
        os.chdir(self.path)
        # run cmt
        result = os.popen4(cmd)[1].read()
        # return to the old directory
        os.chdir(olddir)
        return result
    
    def __getattr__(self,attr):
        return lambda args=[]: self._run_cmt(attr, args)
    
    def runtime_env(self,env = None):
        """Returns a dictionary containing the runtime environment produced by CMT.
        If a dictionary is passed a modified instance of it is returned.
        """
        if env is None:
            env = {}
        for l in self.setup("-csh").splitlines():
            l = l.strip()
            if l.startswith("setenv"):
                dummy,name,value = l.split(None,3)
                env[name] = value.strip('"')
            elif l.startswith("unsetenv"):
                dummy,name = l.split(None,2)
                if name in env:
                    del env[name]
        return env
    def show_macro(self,k):
        r = self.show(["macro",k])
        if r.find("CMT> Error: symbol not found") >= 0:
            return None
        else:
            return self.show(["macro_value",k]).strip()

########################################################################
# Output Validation Classes
########################################################################
class BasicOutputValidator:
    """Basic implementation of an option validator for Gaudi tests.
    This implementation is based on the standard (LCG) validation functions
    used in QMTest.
    """
    def __init__(self,ref,cause,result_key):
        self.reference = ref
        self.cause = cause
        self.result_key = result_key
    
    def __call__(self, out, result):
        """Validate the output of the program.

        'stdout' -- A string containing the data written to the standard output
        stream.

        'stderr' -- A string containing the data written to the standard error
        stream.

        'result' -- A 'Result' object. It may be used to annotate
        the outcome according to the content of stderr.

        returns -- A list of strings giving causes of failure."""

        causes = []
        # Check to see if theoutput matches.
        if not self.__CompareText(out, self.reference):
            causes.append(self.cause) 
            result[self.result_key] = result.Quote(self.reference)

        return causes
    
    def __CompareText(self, s1, s2):
        """Compare 's1' and 's2', ignoring line endings.

        's1' -- A string.

        's2' -- A string.

        returns -- True if 's1' and 's2' are the same, ignoring
        differences in line endings."""

        # The "splitlines" method works independently of the line ending
        # convention in use.
        return s1.splitlines() == s2.splitlines()

class FilePreprocessor:
    """ Base class for a callable that takes a file and returns a modified
    version of it."""
    def __processLine__(self, line):
        return line
    def __call__(self, input):
        if hasattr(input,"__iter__"):
            lines = input
            mergeback = False
        else:
            lines = input.splitlines()
            mergeback = True
        output = []
        for l in lines:
            l = self.__processLine__(l)
            if l: output.append(l)
        if mergeback: output = '\n'.join(output)
        return output
    def __add__(self, rhs):
        return FilePreprocessorSequence([self,rhs])

class FilePreprocessorSequence(FilePreprocessor):
    def __init__(self, members = []):
        self.members = members
    def __add__(self, rhs):
        return FilePreprocessorSequence(self.members + [rhs])
    def __call__(self, input):
        output = input
        for pp in self.members:
            output = pp(output)
        return output

class LineSkipper(FilePreprocessor):
    def __init__(self, strings = [], regexps = []):
        import re
        self.strings = strings
        self.regexps = map(re.compile,regexps)
        
    def __processLine__(self, line):
        for s in self.strings:
            if line.find(s) >= 0: return None
        for r in self.regexps:
            if r.search(line): return None
        return line

class RegexpReplacer(FilePreprocessor):
    def __init__(self, orig, repl = "", when = None):
        if when:
            when = re.compile(when)
        self._operations = [ (when, re.compile(orig), repl) ]
    def __add__(self,rhs):
        if isinstance(rhs, RegexpReplacer):
            res = RegexpReplacer("","",None)
            res._operations = self._operations + rhs._operations
        else:
            res = FilePreprocessor.__add__(self, rhs)
        return res
    def __processLine__(self, line):
        for w,o,r in self._operations:
            if w is None or w.search(line):
                line = o.sub(r, line)
        return line

# Common preprocessors        
maskPointers  = RegexpReplacer("0x[0-9a-fA-F]{4,16}","0x########")
normalizeDate = RegexpReplacer("[0-2]?[0-9]:[0-5][0-9]:[0-5][0-9] [0-9]{4}[-/][01][0-9][-/][0-3][0-9] *(CES?T)?",
                               "00:00:00 1970-01-01")
normalizeEOL = FilePreprocessor()
normalizeEOL.__processLine__ = lambda line: str(line).rstrip() + '\n'

skipEmptyLines = FilePreprocessor()
# FIXME: that's ugly
skipEmptyLines.__processLine__ = lambda line: (line.strip() and line) or None

# Preprocessors for GaudiExamples        
normalizeExamples = maskPointers + normalizeDate 
for w,o,r in [
              #("TIMER.TIMER",r"[0-9]", "0"), # Normalize time output
              ("TIMER.TIMER",r"\s+[+-]?[0-9]+[0-9.]*", " 0"), # Normalize time output
              (r"^\*",r"(File\s*Size\s*=\s*)[0-9]*",r"\1..."), # Normalize ROOT size reports
              (r"^\*",r"(Total\s*(Size)?\s*=\s*)[0-9]*",r"\1..."), # Normalize ROOT size reports
              ("release all pending",r"^.*/([^/]*:.*)",r"\1"),
              ("0x########",r"\[.*/([^/]*.*)\]",r"[\1]"),
              ("^#.*file",r"file '.*[/\\]([^/\\]*)$",r"file '\1"),
              ("^Aida2Root",r"(INFO.*'(skewness|kurtosis)'.*)\|[0-9.e+\- ]*\|",r"\1| ### |"),
              ]: #[ ("TIMER.TIMER","[0-9]+[0-9.]*", "") ]
    normalizeExamples += RegexpReplacer(o,r,w)
normalizeExamples = LineSkipper(["//GP:",
                                 "Time User",
                                 "Welcome to",
                                 "This machine has a speed",
                                 "TIME:",
                                 "running on",
                                 "ToolSvc.Sequenc...   INFO",
                                 "DataListenerSvc      INFO XML written to file:",
                                 "[INFO]","[WARNING]",
                                 "DEBUG No writable file catalog found which contains FID:",
                                 "0 local", # hack for ErrorLogExample
                                 "ROOT::Reflex::NewDelFunctionsT<StatusCode>::delete_T(void*)",
                                 ],regexps = [
                                 r"(Always|SUCCESS)\s*Root file version:", # skip the message reporting the version of the root file
                                 r"0x[0-9a-fA-F#]+ *Algorithm::sysInitialize\(\) *\[", # hack for ErrorLogExample
                                 r"0x[0-9a-fA-F#]* *__gxx_personality_v0 *\[", # hack for ErrorLogExample
                                 r"File '.*.xml' does not exist",
                                 r"INFO Refer to dataset .* by its file ID:",
                                 r"INFO Referring to dataset .* by its file ID:",
                                 r"INFO Disconnect from dataset",
                                 r"INFO Disconnected from dataset",
                                 r"INFO Disconnected data IO:",
                                 r"IncidentSvc\s*(DEBUG (Adding|Removing)|VERBOSE Calling)",
                                 # I want to ignore the header of the unchecked StatusCode report
                                 r"^StatusCodeSvc.*listing all unchecked return codes:",
                                 r"^StatusCodeSvc\s*INFO\s*$",
                                 r"Num\s*|\s*Function\s*|\s*Source Library",
                                 r"^[-+]*\s*$",
                                 ] ) + normalizeExamples + skipEmptyLines + \
                                  normalizeEOL

class ReferenceFileValidator:
    def __init__(self, reffile, cause, result_key, preproc = normalizeExamples):
        self.reffile = os.path.expandvars(reffile)
        self.cause = cause
        self.result_key = result_key
        self.preproc = preproc
    def __call__(self, stdout, result):
        causes = []
        if os.path.isfile(self.reffile):
            orig = open(self.reffile).xreadlines()
            if self.preproc:
                orig = self.preproc(orig)
        else:
            orig = []
        
        new = stdout.splitlines()
        if self.preproc:
            new = self.preproc(new)
        #open(self.reffile + ".test","w").writelines(new)
        diffs = difflib.ndiff(orig,new,charjunk=difflib.IS_CHARACTER_JUNK)
        filterdiffs = map(lambda x: x.strip(),filter(lambda x: x[0] != " ",diffs))
        #filterdiffs = [x.strip() for x in diffs]
        if filterdiffs:
            result[self.result_key] = result.Quote("\n".join(filterdiffs))
            result[self.result_key] += result.Quote("""
Legend:
        -) reference file
        +) standard output of the test""")
            causes.append(self.cause)
            newref = open(self.reffile + ".new","w")
            newref.write(stdout)
            
        return causes

def findReferenceBlock(reference, stdout, result, causes, signature_offset=0, signature=None):
    # split reference file, sanitize EOLs and remove empty lines
    reflines = filter(None,map(lambda s: s.rstrip(), reference.splitlines()))
    # the same on standard output
    outlines = filter(None,map(lambda s: s.rstrip(), stdout.splitlines()))

    if signature is None:
        if signature_offset < 0:
            signature_offset = len(reference)+signature_offset
        signature = reflines[signature_offset]
    # find the reference block in the output file
    try:
        pos = outlines.index(signature)
        outlines = outlines[pos-signature_offset:pos+len(reflines)-signature_offset]
        if reflines != outlines:
            causes.append("standard output")
            result["GaudiTest.observer"] = result.Quote("\n".join(outlines))
    except ValueError:
        causes.append("missing signature")
    result["GaudiTest.signature"] = result.Quote(signature)
    result["GaudiTest.expected"] = result.Quote("\n".join(reflines))

########################################################################
# Test Classes
########################################################################
class GaudiExeTest(ExecTestBase):
    arguments = [
        qm.fields.TextField(
            name="program",
            title="Program",
            not_empty_text=1,
            description="""The path to the program.

            This field indicates the path to the program.  If it is not
            an absolute path, the value of the 'PATH' environment
            variable will be used to search for the program.
            If not specified, $GAUDIEXE or Gaudi.exe are used.
            """
            ),
        qm.fields.SetField(qm.fields.TextField(
            name="args",
            title="Argument List",
            description="""The command-line arguments.

            If this field is left blank, the program is run without any
            arguments.

            Use this field to specify the option files.

            An implicit 0th argument (the path to the program) is added
            automatically."""
            )),
        qm.fields.TextField(
            name="options",
            title="Options",
            description="""Options to be passed to the application.
            
            This field allows to pass a list of options to the main program
            without the need of a separate option file.
            
            The content of the field is written to a temporary file which name
            is passed the the application as last argument (appended to the
            field "Argument List".
            """,
            verbatim="true",
            multiline="true",
            default_value=""
            ),
        qm.fields.TextField(
            name="workdir",
            title="Working Directory",
            description="""Path to the working directory.

            If this field is left blank, the program will be run from the qmtest
            directory, otherwise from the directory specified."""
            ),
        qm.fields.TextField(
            name="reference",
            title="Reference Output",
            description="""Path to the file containing the reference output.

            If this field is left blank, any standard output will be considered
            valid.
            
            If the reference file is specified, any output on standard error is
            ignored."""
            ),
        qm.fields.TextField(
            name="error_reference",
            title="Reference for standard error",
            description="""Path to the file containing the reference for the standard error.

            If this field is left blank, any standard output will be considered
            valid.
            
            If the reference file is specified, any output on standard error is
            ignored."""
            ),
        qm.fields.SetField(qm.fields.TextField(
            name = "unsupported_platforms",
            title = "Unsupported Platforms",
            description = """Platform on which the test must not be run.
            
            List of regular expressions identifying the platforms on which the
            test is not run and the result is set to UNTESTED."""
            )),
                
        qm.fields.TextField(
            name = "validator",
            title = "Validator",
            description = """Function to validate the output of the test.
            
            If defined, the function is used to validate the products of the
            test.
            The function is called passing as arguments:
              self:   the test class instance
              stdout: the standard output of the executed test
              stderr: the standard error of the executed test
              result: the Result objects to fill with messages
            The function must return a list of causes for the failure.
            If specified, overrides standard output, standard error and
            reference files.
            """,
            verbatim="true",
            multiline="true",
            default_value=""
            ),
                
        ]
    
    def PlatformIsNotSupported(self, context, result):
        platform = self.GetPlatform()
        unsupported = [ re.compile(x)
                        for x in [ str(y).strip()
                                   for y in self.unsupported_platforms ]
                        if x
                       ]
        for p_re in unsupported:
            if p_re.search(platform):
                result.SetOutcome(result.UNTESTED)
                result[result.CAUSE] = 'Platform not supported.'
                return True
        return False
    
    def GetPlatform(self):
        """
        Return the platform Id defined in CMTCONFIG or SCRAM_ARCH.
        """
        arch = "None"
        # check architecture name
        if "CMTCONFIG" in os.environ:
            arch = os.environ["CMTCONFIG"]
        elif "SCRAM_ARCH" in os.environ:
            arch = os.environ["SCRAM_ARCH"]
        return arch
        
    def ValidateOutput(self, stdout, stderr, result):
        causes = []
        # if the test definition contains a custom validator, use it
        if self.validator.strip() != "":
            exec self.validator in globals(), {"self":self,
                                               "stdout":stdout,
                                               "stderr":stderr,
                                               "result":result,
                                               "causes":causes}
            return causes
        
        # check standard output
        validator = None
        if self.reference:
            # first check for the os specific ref file
            reference = os.path.normpath(os.path.expandvars(self.reference))
            spec_ref = reference[:-3] + self.GetPlatform()[0:3] + reference[-3:]
            if os.path.isfile(spec_ref):
                result["GaudiTest.output_reference"] = spec_ref
                validator = ReferenceFileValidator(spec_ref,"standard output","GaudiTest.output_diff")
            elif os.path.isfile(reference):
                result["GaudiTest.output_reference"] = reference
                validator = ReferenceFileValidator(reference,"standard output","GaudiTest.output_diff")
        if not (validator is None):
            causes += validator(stdout,result)
        
        # check standard error
        validator = None
        if self.error_reference:
            # first check for the os specific ref file
            reference = os.path.normpath(os.path.expandvars(self.error_reference))
            spec_ref = reference[:-3] + self.GetPlatform()[0:3] + reference[-3:]
            if os.path.isfile(spec_ref):
                result["GaudiTest.error_reference"] = spec_ref
                validator = ReferenceFileValidator(spec_ref,"standard error","GaudiTest.error_diff")
            elif os.path.isfile(reference):
                result["GaudiTest.error_reference"] = reference
                validator = ReferenceFileValidator(reference,"standard error","GaudiTest.error_diff")
        if validator is None: # If we cannot use the reference file fall back to basic
            validator = BasicOutputValidator(self.stderr,"standard error","ExecTest.expected_stderr")
        causes += validator(stderr,result)
        
        return causes
    
    def _find_program(self,prog):
        # check if it is an absolute path or the file can be found
        # from the local directory, otherwise search for it in PATH
        if not os.path.isabs(prog) and not os.path.isfile(prog):
            for d in os.environ["PATH"].split(os.pathsep):
                p = os.path.join(d,prog)
                if os.path.isfile(p):
                    return p
        return prog
        
    def Run(self, context, result):
        """Run the test.

        'context' -- A 'Context' giving run-time parameters to the
        test.

        'result' -- A 'Result' object.  The outcome will be
        'Result.PASS' when this method is called.  The 'result' may be
        modified by this method to indicate outcomes other than
        'Result.PASS' or to add annotations."""
        
        # Prepare the working directory
        #tempdir = TempDir(chdir = True)
        
        # Check if the platform is supported
        if self.PlatformIsNotSupported(context, result):
            return
        
        # Prepare program name and arguments (expanding variables) 
        if self.program:
            prog = os.path.normpath(os.path.expandvars(self.program))
        elif "GAUDIEXE" in os.environ:
            prog = os.environ["GAUDIEXE"]
        else:
            prog = "Gaudi.exe"
        self.program = prog
            
        dummy, prog_ext = os.path.splitext(prog)
        if prog_ext not in [ ".exe", ".py", ".bat" ] and self.GetPlatform()[0:3] == "win":
            prog += ".exe"
            prog_ext = ".exe"
            
        prog = self._find_program(prog)
        
        args = map(lambda x: os.path.normpath(os.path.expandvars(x)),
                   self.args)
        
        # check if the user provided inline options
        tmpfile = None
        if self.options.strip():
            ext = ".opts"
            if re.search(r"from\s*Gaudi.Configuration\s*import\s*\*", self.options):
                ext = ".py"
            tmpfile = TempFile(ext)
            tmpfile.writelines("\n".join(self.options.splitlines()))
            tmpfile.flush()
            args.append(tmpfile.name)
            result["GaudiTest.options"] = result.Quote(self.options)
        
        # if the program is a python file, execute it through python
        if prog_ext == ".py":
            args.insert(0,prog)
            if self.GetPlatform()[0:3] == "win":
                prog = self._find_program("python.exe")
            else:
                prog = self._find_program("python")
        
        # Change to the working directory if specified
        origdir = None
        if self.workdir:
            origdir = os.getcwd()
            os.chdir(str(os.path.normpath(os.path.expandvars(self.workdir))))
        
        if "QMTEST_IGNORE_TIMEOUT" not in os.environ:
            self.timeout = max(self.timeout,600)
        else:
            self.timeout = -1
        
        try:
            # Run the test
            self.RunProgram(prog, 
                            [ str(self.program) ] + args,
                            context, result)
        finally:
            # revert to the original directory (if we moved away)
            if origdir:
                os.chdir(origdir)
        
    def RunProgram(self, program, arguments, context, result):
        """Run the 'program'.

        'program' -- The path to the program to run.

        'arguments' -- A list of the arguments to the program.  This
        list must contain a first argument corresponding to 'argv[0]'.

        'context' -- A 'Context' giving run-time parameters to the
        test.

        'result' -- A 'Result' object.  The outcome will be
        'Result.PASS' when this method is called.  The 'result' may be
        modified by this method to indicate outcomes other than
        'Result.PASS' or to add annotations.
        
        @attention: This method has been copied from command.ExecTestBase
                    (QMTest 2.3.0) and modified to keep stdout and stderr
                    for tests that have been terminated by a signal.
                    (Fundamental for debugging in the Application Area)
        """

        # Construct the environment.
        environment = self.MakeEnvironment(context)
        # Create the executable.
        if self.timeout >= 0:
            timeout = self.timeout
        else:
            # If no timeout was specified, we sill run this process in a
            # separate process group and kill the entire process group
            # when the child is done executing.  That means that
            # orphaned child processes created by the test will be
            # cleaned up.
            timeout = -2
        e = qm.executable.Filter(self.stdin, timeout)
        # Run it.
        exit_status = e.Run(arguments, environment, path = program)

        # If the process terminated normally, check the outputs.
        if sys.platform == "win32" or os.WIFEXITED(exit_status):
            # There are no causes of failure yet.
            causes = []
            # The target program terminated normally.  Extract the
            # exit code, if this test checks it.
            if self.exit_code is None:
                exit_code = None
            elif sys.platform == "win32":
                exit_code = exit_status
            else:
                exit_code = os.WEXITSTATUS(exit_status)
            # Get the output generated by the program.
            stdout = e.stdout
            stderr = e.stderr
            # Record the results.
            result["ExecTest.exit_code"] = str(exit_code)
            result["ExecTest.stdout"] = result.Quote(stdout)
            result["ExecTest.stderr"] = result.Quote(stderr)
            # Check to see if the exit code matches.
            if exit_code != self.exit_code:
                causes.append("exit_code")
                result["ExecTest.expected_exit_code"] \
                    = str(self.exit_code)
            # Validate the output.
            causes += self.ValidateOutput(stdout, stderr, result)
            # If anything went wrong, the test failed.
            if causes:
                result.Fail("Unexpected %s." % string.join(causes, ", ")) 
        elif os.WIFSIGNALED(exit_status):
            # The target program terminated with a signal.  Construe
            # that as a test failure.
            signal_number = str(os.WTERMSIG(exit_status))
            result.Fail("Program terminated by signal.")
            result["ExecTest.signal_number"] = signal_number
            result["ExecTest.stdout"] = result.Quote(e.stdout)
            result["ExecTest.stderr"] = result.Quote(e.stderr)
        elif os.WIFSTOPPED(exit_status):
            # The target program was stopped.  Construe that as a
            # test failure.
            signal_number = str(os.WSTOPSIG(exit_status))
            result.Fail("Program stopped by signal.")
            result["ExecTest.signal_number"] = signal_number
            result["ExecTest.stdout"] = result.Quote(e.stdout)
            result["ExecTest.stderr"] = result.Quote(e.stderr)
        else:
            # The target program terminated abnormally in some other
            # manner.  (This shouldn't normally happen...)
            result.Fail("Program did not terminate normally.")
        
        # Marco Cl.: This is a special trick to fix a "problem" with the output
        # of gaudi jobs when they use colors
        esc = '\x1b'
        repr_esc = '\\x1b'
        result["ExecTest.stdout"] = result["ExecTest.stdout"].replace(esc,repr_esc)
        # TODO: (MCl) improve the hack for colors in standard output
        #             may be converting them to HTML tags
