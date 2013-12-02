#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"

os.execute("rm -rf deploy && mkdir deploy")

os.execute("cp -r `ls | grep -v deploy` deploy")

os.execute("rm -rf deploy/Den* deploy/TODO deploy/projects deploy/bin deploy/lib")

os.execute("cd deploy && scripts/generate.lua")

-- stash the projects away so we can retrieve them after we've built
os.execute("cp -r deploy/projects deploy/projectsbackup")

os.execute("cd deploy && scripts/build.lua")

-- retrieve the projects
os.execute("rm -rf deploy/projects && mv deploy/projectsbackup deploy/projects")

-- remove .idb's
os.execute("rm -rf deploy/lib/*/*.idb")

-- remove binary output - we'll make this a little more careful if we have deployable executables
os.execute("rm -rf deploy/bin")

-- compress the whole shebang
os.execute("mv deploy frames-`git describe | sed s/v//`")
os.execute("zip -r -9 ../frames-`git describe | sed s/v//`.zip frames-`git describe | sed s/v//`")
os.execute("rm -rf frames-`git describe | sed s/v//`")
