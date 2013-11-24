#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"

for k, v in pairs(projects) do
  os.execute(("cd projects/%s && %s"):format(k, v.build))
end
