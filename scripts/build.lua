#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"
require "scripts/lib/util"

local typ = ...

local src = projects
if typ then
  src = {}
  src[typ] = projects[typ]
end

local success = true

for k, v in pairs(src) do
  if os.execute(("cd projects/%s && %s"):format(k, v.build)) ~= 0 then
    success = false
  end
end

os.exit(success and 0 or 1)
