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

require "scripts/lib/util"

local version = io.open("version", "rb"):read("*line")

os.execute("rm -rf Den* scripts version TODO")

os.execute(("mkdir frames-%s && mv * frames-%s"):format(version, version))

-- I never used the "binary deploy" option, so I'm just pulling it
--[[
-- compress the whole shebang with binaries
os.execute(("zip -r -9 -q frames-%s-bin.zip frames-%s"):format(version, version))]]

-- remove binary output - we'll make this a little more careful if we later have deployable executables
os.execute(("rm -rf frames-%s/bin"):format(version))

-- create the final deployable
os.execute(("zip -r -9 -q frames-%s.zip frames-%s"):format(version, version))
