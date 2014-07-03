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

local target = ...

os.execute("rm -rf doc/Frames* doc/html")

os.execute('"/cygdrive/c/Program Files/doxygen/bin/doxygen.exe" doc/Doxyfile')

os.execute('cd doc/html ; "/cygdrive/c/Program Files (x86)/HTML Help Workshop/hhc" index.hhp ; mv index.chm ../Frames.chm')

os.execute("rm -rf doc/html")
  
os.execute('(cat doc/Doxyfile; echo GENERATE_HTMLHELP = NO) | "/cygdrive/c/Program Files/doxygen/bin/doxygen.exe" -')
