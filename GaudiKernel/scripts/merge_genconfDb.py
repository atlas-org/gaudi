#!/usr/bin/env python

# @file: GaudiPolicy/cmt/fragments/merge_genconfDb.py
# @purpose: merge <library>_confDb.py files into a 'per-project' confDb.py file
# @author: Sebastien Binet <binet@cern.ch>

import os
import sys
from datetime import datetime

def usage(sc = 1) :
    print 'Usage:'
    print '  merge_genconfDb <confDbFileName> <project_merged_confDbFileName>'
    print '  return status',sc
    sys.exit(sc)

import mmap
def mergeConfDbFiles( libConfDbFileName, projConfDbFileName ):
    confDb = os.path.basename( libConfDbFileName ).replace( ".py", "" )
    startMark = "#--Begin %s" % ( confDb + os.linesep )
    timeMark  = "#--Date inserted: %s" % ( str(datetime.now()) + os.linesep )
    endMark   = "#--End %s" % ( confDb + os.linesep )

    try:
        newLines = []
        projConfDb   = open( projConfDbFileName, 'r+' )
        projFileSize = os.path.getsize( projConfDb.name )

        if projFileSize > 0:
            memProjConfDb = mmap.mmap( projConfDb.fileno(),
                                       projFileSize,
                                       access = mmap.ACCESS_WRITE )
            startMarkIdx = memProjConfDb.find(startMark)
            endMarkIdx   = memProjConfDb.find(endMark)
            if startMarkIdx != -1 and endMarkIdx != -1:
                newLines = memProjConfDb[:startMarkIdx].strip() + \
                           memProjConfDb[endMarkIdx+
                                         len(endMark):].strip()
            else:
                newLines = memProjConfDb[:].strip()
            newLines = newLines.split(os.linesep)
            memProjConfDb.close()
            pass

        projConfDb.seek(0)
        projConfDb.truncate(0)
        if len(newLines) > 0:
            projConfDb.writelines( os.linesep.join(newLines) )
            pass
        projConfDb.writelines(
            os.linesep +
            startMark  +
            timeMark   +
            "".join( [l for l in open( libConfDbFileName, 'r' ) ] ) +
            endMark
            )
    except IOError, err:
        print "ERROR:",err
        usage(2)
    return

if __name__ == "__main__":
    mergeConfDbFiles( libConfDbFileName  = sys.argv[1],
                      projConfDbFileName = sys.argv[2] )
    
    sys.exit(0)
