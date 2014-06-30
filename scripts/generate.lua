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

if src.ue4_2 then
  os.execute("rm -rf ue4/Binaries ue4/Intermediate ue4/Plugins/Frames/Binaries ue4/Plugins/Frames/Intermediate ue4/*.sln")
end

for k, v in pairs(src) do
  os.execute(("util/premake/win/premake5 %s %s"):format(v.parameters or "", v.generator))
end
