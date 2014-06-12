#!env lua

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

-- NOTE: A lot of this assumes linear development and no going back and adding bugfixes. When I start doing that, I'll need to revamp some of this.

require "scripts/lib/util"

do
  local gitfil = io.popen("git status --porcelain", "r")
  local output = gitfil:read("*all")
  if #output ~= 0 then
    print("Error - git repo contains data that has not been committed!")
    assert(false)
  end
end

local ver = ...

print(ver)
assert(ver)

do
  local gitfil = io.popen("git tag", "r")
  local output = gitfil:read("*all")
  if output:find("v" .. ver .. "\n") then
    print("Error - tag is already in use!")
    assert(false)
  end
end

do
  local gitfil = io.popen("git branch", "r")
  local output = gitfil:read("*all")
  if not output:find("* dev\n") then
    print("Error - not on dev branch!")
    assert(false)
  end
end

local major, minor, patch, suffix = ver:match("^(%d+).(%d+).(%d+)(-?.*)$")
if suffix == "" then
  suffix = nil
end

assert(not suffix or suffix:match("-.*"))
assert(major and minor and patch)

print(major, minor, patch, suffix)

-- set up our doc branch
os.execute("git clone . tagtemp")
os.execute("cd tagtemp && git checkout gh-pages")

-- get documentation built
os.execute("lua scripts/generateDocs.lua")

local function branchto(branch)
  os.execute("git branch -f " .. branch)
  os.execute("git config branch." .. branch .. ".remote origin")
  os.execute("git config branch." .. branch .. ".merge refs/heads/" .. branch)
  os.execute("rm -rf tagtemp/docs/" .. branch)
  os.execute("mkdir -p tagtemp/docs/" .. branch)
  os.execute("cp -r doc/html/* tagtemp/docs/" .. branch)
end

if not suffix then
  if tonumber(major) > 0 then
    -- Push to stable
    branchto("stable")
  end
  
  -- Push to test
  branchto("test")

  -- Push to vM branch
  branchto("v" .. major)
  
  -- Push to vM.N branch
  branchto("v" .. major .. "." .. minor)
end

os.execute([[cd tagtemp && git add . && git commit -a -m "Documentation update for version v]] .. ver .. [[" && git push && cd .. && rm -rf tagtemp]])

os.execute("git tag v" .. ver .. " -m \"Version " .. ver .. "\"")
