from Gaudi.Configuration import *

print "#### This is 'test1.py'"

MessageSvc().OutputLevel = WARNING

ApplicationMgr().TopAlg += [ str(x) for x in range(10) ]
