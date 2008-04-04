#!/usr/bin/env python

#---------------------------------------------------------------------
if __name__ == "__main__":
    import sys
    from Gaudi import gaudimain
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] <opts_file> ...")
    parser.add_option("-n","--dry-run", action="store_true",
                      help="do not run the application, just parse option files")
    parser.add_option("-p","--pickle-output", action="store", type="string",
                      metavar = "FILE",
                      help="write the parsed options as a pickle file (static option file)")
    parser.add_option("-v","--verbose", action="store_true",
                      help="print the parsed options")
    parser.add_option("--old-opts", action="store_true",
                      help="format printed options in old option files style")
    parser.add_option("--all-opts", action="store_true",
                      help="print all the option (even if equal to default)")
    
    opts, args = parser.parse_args() 
    
    c = gaudimain()
    c.configure(args)
    
    if opts.verbose:
        c.dumpconfig(opts.old_opts,opts.all_opts)
    if opts.pickle_output:
        c.writepickle(opts.pickle_output)
    if not opts.dry_run:
        c.run()
