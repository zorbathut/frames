#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

os.execute("rm -rf projects")

local projects = {
  msvc9 = {generator = "Visual Studio 9 2008"},
  msvc10 = {generator = "Visual Studio 10"},
  msvc11 = {generator = "Visual Studio 11"},
  msvc12 = {generator = "Visual Studio 12"},
  mingw = {generator = "MinGW Makefiles", path = "/cygdrive/c/mingw/bin"},
}

--[[for _, k in ipairs({"msvc10", "msvc11", "msvc12", "msvc9", "mingw"}) do
  local v = projects[k] ]]
for k, v in pairs(projects) do
  local path = v.path and ("PATH=\"%s\""):format(v.path) or ""
  os.execute(("mkdir -p projects/%s && cd projects/%s && %s /cygdrive/c/Program\\ Files\\ \\(x86\\)/CMake\\ 2.8/bin/cmake.exe -G '%s' ../.."):format(k, k, path, v.generator))
end
