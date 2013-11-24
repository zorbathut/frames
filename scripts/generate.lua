#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"

os.execute("rm -rf projects")

for k, v in pairs(projects) do
  local path = v.path and ("PATH=\"%s\""):format(v.path) or ""
  os.execute(("mkdir -p projects/%s && cd projects/%s && %s /cygdrive/c/Program\\ Files\\ \\(x86\\)/CMake\\ 2.8/bin/cmake.exe -G '%s' ../.."):format(k, k, path, v.generator))
end
