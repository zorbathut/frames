#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

local version = io.open("version", "rb"):read("*line")

os.execute("rm -rf Den* scripts bin version")

os.execute(("mkdir frames-%s && mv * frames-%s"):format(version, version))

-- compress the whole shebang with binaries
os.execute(("zip -r -9 -q frames-%s-bin.zip frames-%s"):format(version, version))

-- remove binary output - we'll make this a little more careful if we later have deployable executables
os.execute(("rm -rf frames-%s/bin"):format(version))

-- create the final deployable
os.execute(("zip -r -9 -q frames-%s.zip frames-%s"):format(version, version))
