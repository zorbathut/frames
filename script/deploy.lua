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

require "script/lib/util"
require "script/lib/platforms"

local version = io.open("version", "rb"):read("*line")
local major, minor, patch, update = version:match("^(%d+)%.(%d+)%.(%d+)-(%d+)-")

if not major then
  major, minor, patch = version:match("^(%d+)%.(%d+)%.(%d+)$")
  update = 0
end

assert(major and minor and patch and update)
local uev = ((major * 100 + minor) * 100 + patch) * 1000 + update

os.execute("rm -rf Den* script version TODO")

-- create ue4 deployments
for v in pairs(projects) do
  if v:find("ue4") then
    os.execute(("mkdir -p plugin/%s"):format(v))
    os.execute(("cp -r ue4/Plugins plugin/%s"):format(v))
    os.execute(("mkdir -p plugin/%s/Plugins/Frames/Source/ThirdParty/FramesDeps/deps"):format(v))
    os.execute(("mkdir -p plugin/%s/Plugins/Frames/Source/ThirdParty/FramesDeps/lib"):format(v))
    os.execute(("cp -r include plugin/%s/Plugins/Frames/Source/ThirdParty/FramesDeps"):format(v))
    os.execute(("mv lib/%s plugin/%s/Plugins/Frames/Source/ThirdParty/FramesDeps/lib"):format(v, v))
    os.execute(("rm plugin/%s/Plugins/Frames/Source/ThirdParty/FramesDeps/lib/%s/x32/frames_renderer*"):format(v, v))
    os.execute(("rm plugin/%s/Plugins/Frames/Source/ThirdParty/FramesDeps/lib/%s/x64/frames_renderer*"):format(v, v))
    os.execute(("cp -r deps/boost_1_55_0 deps/jpeg-9 plugin/%s/Plugins/Frames/Source/ThirdParty/FramesDeps/deps"):format(v))
    os.execute(("sed -i s/DEVELOPMENT/%s/ plugin/%s/Plugins/Frames/Frames.uplugin"):format(version, v))
    os.execute(("sed -i 's/: 1,/: %d,/' plugin/%s/Plugins/Frames/Frames.uplugin"):format(uev, v))
  end
end

os.execute(("mkdir frames-%s && mv * frames-%s"):format(version, version))

-- I never used the "binary deploy" option, so I'm just pulling it
--[[
-- compress the whole shebang with binaries
os.execute(("zip -r -9 -q frames-%s-bin.zip frames-%s"):format(version, version))]]

-- remove binary output - we'll make this a little more careful if we later have deployable executables
os.execute(("rm -rf frames-%s/bin"):format(version))

-- create the final deployable
os.execute(("zip -r -9 -q frames-%s.zip frames-%s"):format(version, version))
