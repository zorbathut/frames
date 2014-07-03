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

require "script/lib/platforms"
require "script/lib/util"

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
  if os.execute(("cd project/%s && %s"):format(k, v.build(platform, configuration))) ~= 0 then
    success = false
  end
end

os.exit(success and 0 or 1)
