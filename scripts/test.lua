#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"

local failure = false

local function ose(cmd)
  print(cmd)
  return os.execute(cmd)
end

for k, v in pairs(projects) do
  if ose(("cd test && ../bin/%s/test/test.exe"):format(k)) ~= 0 then failure = true end
  if ose(("cd test && ../bin/%s/test/testd.exe"):format(k)) ~= 0 then failure = true end
end

os.exit(failure and 1 or 0)
