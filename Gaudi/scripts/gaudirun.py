#!/usr/bin/env python

#---------------------------------------------------------------------
if __name__ == "__main__":
    import sys
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] <opts_file> ...")
    parser.add_option("-n","--dry-run", action="store_true",
                      help="do not run the application, just parse option files")
    parser.add_option("-p","--pickle-output", action="store", type="string",
                      metavar = "FILE",
                      help="DEPRECATED: use '--output file.pkl' instead. Write "
                           "the parsed options as a pickle file (static option "
                           "file)")
    parser.add_option("-v","--verbose", action="store_true",
                      help="print the parsed options")
    parser.add_option("--old-opts", action="store_true",
                      help="format printed options in old option files style")
    parser.add_option("--all-opts", action="store_true",
                      help="print all the option (even if equal to default)")
    def option_cb(option, opt, value, parser):
        """Add the option line to a list together with its position in the
        argument list.
        """
        parser.values.options.append((len(parser.largs), value))
    parser.add_option("--option", action="callback", callback=option_cb,
                      type = "string", nargs = 1,
                      help="add a single line (Python) option to the configuration." +
                           "All options lines are executed, one after the other, in " +
                           "the same context.")
    parser.add_option("--no-conf-user-apply", action="store_true",
                      help="disable the automatic application of configurable "
                           "users (for backward compatibility)")
    parser.add_option("-o", "--output", action = "store", type = "string",
                      help ="dump the configuration to a file. The format of "
                            "the options is determined by the extension of the "
                            "file name: .pkl = pickle, .py = python, .opts = "
                            "old style options. The python format cannot be "
                            "used to run the application and it contains the "
                            "same dictionary printed with -v")
    parser.add_option("--post-option", action="append", type="string",
                      dest="post_options",
                      help="Python options to be executed after the ConfigurableUser "
                           "are applied." 
                           "All options lines are executed, one after the other, in " +
                           "the same context.")
    parser.add_option("--debug", action="store_true",
                      help="enable some debug print-out")

    parser.set_defaults(options = [])

    opts, args = parser.parse_args() 
    
    # configure the logging
    import logging
    from GaudiKernel.ProcessJobOptions import InstallRootLoggingHandler
    
    if opts.old_opts: prefix = "// "
    else: prefix = "# "
    level = logging.INFO
    if opts.debug:
        level = logging.DEBUG
    InstallRootLoggingHandler(prefix, level = level)
    root_logger = logging.getLogger()
    
    if opts.pickle_output:
        if opts.output:
            root_logger.error("Conflicting options: use only --pickle-output or --output")
            sys.exit(1)
        else:
            root_logger.warning("--pickle-output is deprecated, use --output instead")
            opts.output = opts.pickle_output
    
    from Gaudi import gaudimain
    c = gaudimain()

    # Prepare the "configuration script" to parse (like this it is easier than
    # having a list with files and python commands, with an if statements that
    # decides to do importOptions or exec)
    options = [ "importOptions(%r)" % f for f in args ]
    # The option lines are inserted into the list of commands using their
    # position on the command line 
    optlines = list(opts.options)
    optlines.reverse() # this allows to avoid to have to care about corrections of the positions
    for pos, l in optlines:
        options.insert(pos,l)
    
    # "execute" the configuration script generated (if any)
    if options:
        g = {}
        l = {}
        exec "from Gaudi.Configuration import *" in g, l
        for o in options:
            logging.debug(o)
            exec o in g, l
    
    import GaudiKernel.Proxy.Configurable
    if opts.no_conf_user_apply:
        logging.info("Disabling automatic apply of ConfigurableUser")
        # pretend that they have been already applied
        GaudiKernel.Proxy.Configurable._appliedConfigurableUsers_ = True
    
    # This need to be done before dumping
    from GaudiKernel.Proxy.Configurable import applyConfigurableUsers
    applyConfigurableUsers()
    
    # Options to be processed after applyConfigurableUsers
    if opts.post_options:
        g = {}
        l = {}
        exec "from Gaudi.Configuration import *" in g, l
        for o in opts.post_options:
            logging.debug(o)
            exec o in g, l
    
    if opts.verbose:
        c.printconfig(opts.old_opts, opts.all_opts)
    if opts.output:
        c.writeconfig(opts.output, opts.all_opts)
    if not opts.dry_run:
        sys.exit(c.run())
