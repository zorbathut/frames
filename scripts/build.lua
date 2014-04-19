#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"
require "scripts/lib/util"

local target, platform, configuration = ...

local src = projects
if target then
  src = {}
  src[target] = projects[target]
end

if not platform then
  platform = "x32"
end

if not configuration then
  configuration = "release"
end

local success = true

for k, v in pairs(src) do
  if os.execute(("cd projects/%s && %s"):format(k, v.build(platform, configuration))) ~= 0 then
    success = false
  end
end

os.exit(success and 0 or 1)
