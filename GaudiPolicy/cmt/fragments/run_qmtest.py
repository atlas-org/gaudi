#!/usr/bin/env python
#
# Small wrapper script to simplify the execution of QMTest tests.
#
# @author: Marco Clemencic <marco.clemencic@cern.ch>
# @date: 22/10/2007
#
import os, sys

package = "Unknown"
qmtest_args = []
have_user_options = False
if len(sys.argv) > 1:
    package = sys.argv[1]
    if len(sys.argv) > 2:
        qmtest_args = sys.argv[2:]
        have_user_options = True

print "==========> Running tests for package %s"%package

qmtest_dir = os.path.normpath(os.path.expandvars(os.environ["QMTESTLOCALDIR"]))

# check if the user specified a different output file
output_pos = -1
for i in range(len(qmtest_args)):
    if qmtest_args[i] in ['-o','--output']:
        output_pos = i + 1
        break
if output_pos != -1:
    if output_pos < len(qmtest_args):
        # make the path absolute 
        qmtest_args[output_pos] = os.path.realpath(qmtest_args[output_pos])
else:
    # otherwise add the default
    qmtest_results = os.path.normpath(os.path.expandvars(os.environ["QMTESTRESULTS"]))
    qmtest_args = ["-o",qmtest_results] + qmtest_args

cmd = "qmtest run %s"%(" ".join(qmtest_args))

print "==========> Entering '%s'"%qmtest_dir
os.chdir(qmtest_dir)

if not os.path.isdir("QMTest"):
    # The QMTest database is not initialized
    print "==========> Initializing QMTest database"
    os.system("qmtest create-tdb")

# check if we have a test suite called as the package
if not have_user_options and os.path.exists("%s.qms"%package.lower()):
    cmd += " %s"%package.lower()

print "==========> Running '%s'"%cmd
os.system(cmd)
# Note: the return code od qmtest is not propagated to aviod that
#       CMT stops if we have a non-PASS tests (e.g. UNTESTED).
