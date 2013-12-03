#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

require "scripts/lib/platforms"

local typ = ...

local src = projects
if typ then
  src = {}
  src[typ] = projects[typ]
end

for k, v in pairs(src) do
  os.execute(("cd projects/%s && %s"):format(k, v.build))
end
