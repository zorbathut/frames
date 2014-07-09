#!env lua

--[[Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. ]]

-- NOTE: This script currently makes a pile of unwarranted assumptions about the build environment.
-- This is not a good solution and will be fixed when it becomes more important.

-- NOTE: A lot of this assumes linear development and no going back and adding bugfixes. When I start doing that, I'll need to revamp some of this.

require "script/lib/util"

--[[ do -- This is a nice idea, but doesn't work properly with cygwin's somewhat flaky git behavior, which is what I'm using for reasons that are becoming increasingly unclear to me
  local gitfil = io.popen("git status --porcelain", "r")
  local output = gitfil:read("*all")
  if #output ~= 0 then
    print("Error - git repo contains data that has not been committed!")
    assert(false)
  end
end]]

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
os.execute("lua script/generateDocs.lua")

local function docto(tag)
  os.execute("rm -rf tagtemp/docs/" .. tag)
  os.execute("mkdir -p tagtemp/docs/" .. tag)
  os.execute("cp -r doc/html/* tagtemp/docs/" .. tag)
end

local function branchto(branch)
  os.execute("git branch -f " .. branch)
  os.execute("git config branch." .. branch .. ".remote origin")
  os.execute("git config branch." .. branch .. ".merge refs/heads/" .. branch)
  
  docto(branch)
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

-- Add docs
docto("v" .. ver);

os.execute([[cd tagtemp && git add -A . && git commit -a -m "Documentation update for version v]] .. ver .. [[" && git push && cd .. && rm -rf tagtemp]])

os.execute("git tag v" .. ver .. " -m \"Version " .. ver .. "\"")
