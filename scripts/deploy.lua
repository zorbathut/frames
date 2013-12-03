#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"

os.execute("rm -rf deploy && mkdir deploy")

os.execute("cp -r `ls | grep -v deploy` deploy")

os.execute("rm -rf deploy/Den* deploy/TODO deploy/projects deploy/bin deploy/lib deploy/*.zip")

os.execute("cd deploy && scripts/generate.lua")

-- stash the projects away so we can retrieve them after we've built
os.execute("cp -r deploy/projects deploy/projectsbackup")

os.execute("cd deploy && scripts/build.lua")

-- retrieve the original projects
os.execute("rm -rf deploy/projects && mv deploy/projectsbackup deploy/projects")

-- remove .idb's
os.execute("rm -rf deploy/lib/*/*.idb")

-- run all tests
local testfailed = os.execute("cd deploy && scripts/test.lua")

-- move to new location
os.execute("mv deploy frames-`git describe | sed s/v//`")

-- compress the whole shebang with binaries
os.execute("zip -r -9 -q frames-`git describe | sed s/v//`-bin.zip frames-`git describe | sed s/v//`")

-- remove binary output - we'll make this a little more careful if we have deployable executables
os.execute("rm -rf deploy/bin")

-- create the final deployable
os.execute("zip -r -9 -q frames-`git describe | sed s/v//`.zip frames-`git describe | sed s/v//`")
os.execute("rm -rf frames-`git describe | sed s/v//`")

os.exit(testfailed and 1 or 0)
