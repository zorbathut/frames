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
  os.execute(string.format("rm -rf projects/%s lib/%s bin/%s", typ, typ, typ))
else
  os.execute("rm -rf projects lib bin")
end

for k, v in pairs(projects) do
  --local path = v.path and ("PATH=\"%s\""):format(v.path) or ""
  os.execute(("util/premake/win/premake5 %s %s"):format(v.parameters or "", v.generator))
end
