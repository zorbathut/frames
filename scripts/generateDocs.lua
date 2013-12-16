#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/util"

os.execute("rm -rf doc/Frames* doc/html")

os.execute("doxygen doc/Doxyfile")

os.execute('cd doc/html ; "/cygdrive/c/Program Files (x86)/HTML Help Workshop/hhc" index.hhp ; mv index.chm ../Frames.chm ; rm index.hh*')
