#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"

os.execute("rm -rf projects lib bin")

for k, v in pairs(projects) do
  --local path = v.path and ("PATH=\"%s\""):format(v.path) or ""
  os.execute(("premake4 %s"):format(v.generator))
end
