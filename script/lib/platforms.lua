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

local function msbuild(version)
  return function (target, platform, configuration)
    local platlookup = {
      x32 = "Win32",
      x64 = "x64",
    }
    local conflookup = {
      release = "Release",
      debug = "Debug",
    }
    
    -- vs2008 doesn't seem to like multiple environment variables that are identical when compared case-insensitively
    -- because we're lazy, we strip them for all compilers
    return {cli = string.format([[tmp= temp= cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio %s/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=%s /p:platform=%s]],
      version, conflookup[configuration], platlookup[platform]),
      verify = {string.format("lib/%s/%s/%s", target, platform, configuration == "release" and "frames.lib" or "framesd.lib")}
    }
  end
end

local function uebuild(version)
  return function (target, platform, configuration)
    local msb = msbuild("12.0")(target, platform, configuration)
    
    local ueplatform
    if platform == "x64" then
      ueplatform = "Win64"
    elseif platform == "x32" then
      ueplatform = "Win32"
    else
      assert(false)
    end
    
    -- THINGS TO DEAL WITH:
    -- Most of the removed options are removed because we currently build against precompiled binaries
    -- If we shift to a source build we'll change this and test everything possible, but that introduces some logistical difficulties
    -- Debug (not tested)
    -- DebugGame (x64 client, x64 editor, as "debug")
    -- Development (x64 client, x64 editor, as "release")
    -- Shipping (x32 client, as "release")
    
    if configuration == "release" then
      if platform == "x64" then
        msb.cli = msb.cli .. string.format([[ "&&" "C:\Program Files\Unreal Engine\%s\Engine\Build\BatchFiles\Build.bat" plugin_ue4Editor %s Development %%CD%%/../../ue4/plugin_ue4.uproject -rocket]], version, ueplatform)
        table.insert(msb.verify, ("ue4/Plugins/Frames/Binaries/%s/UE4Editor-Frames.dll"):format(ueplatform))
        
        msb.cli = msb.cli .. string.format([[ "&&" "C:\Program Files\Unreal Engine\%s\Engine\Build\BatchFiles\Build.bat" plugin_ue4 %s Development %%CD%%/../../ue4/plugin_ue4.uproject -rocket]], version, ueplatform)
        table.insert(msb.verify, ("ue4/Binaries/%s/plugin_ue4.lib"):format(ueplatform, ueplatform))
      end
      
      if platform == "x32" then
        msb.cli = msb.cli .. string.format([[ "&&" "C:\Program Files\Unreal Engine\%s\Engine\Build\BatchFiles\Build.bat" plugin_ue4 %s Shipping %%CD%%/../../ue4/plugin_ue4.uproject -rocket]], version, ueplatform)
        table.insert(msb.verify, ("ue4/Binaries/%s/plugin_ue4-%s-Shipping.lib"):format(ueplatform, ueplatform))
      end
    elseif configuration == "debug" then
      if platform == "x64" then
        msb.cli = msb.cli .. string.format([[ "&&" "C:\Program Files\Unreal Engine\%s\Engine\Build\BatchFiles\Build.bat" plugin_ue4Editor %s DebugGame %%CD%%/../../ue4/plugin_ue4.uproject -rocket]], version, ueplatform)
        table.insert(msb.verify, ("ue4/Plugins/Frames/Binaries/%s/UE4Editor-Frames-%s-DebugGame.dll"):format(ueplatform, ueplatform))
        
        msb.cli = msb.cli .. string.format([[ "&&" "C:\Program Files\Unreal Engine\%s\Engine\Build\BatchFiles\Build.bat" plugin_ue4 %s DebugGame %%CD%%/../../ue4/plugin_ue4.uproject -rocket]], version, ueplatform)
        table.insert(msb.verify, ("ue4/Binaries/%s/plugin_ue4-%s-DebugGame.lib"):format(ueplatform, ueplatform))
      end
      
      if platform == "x32" then
        
      end
    else
      assert(false)
    end
    
    return msb
  end
end

projects = {
  msvc9 = {
    generator = "vs2008",
    build = msbuild("9.0"),
  },
  msvc10 = {
    generator = "vs2010",
    build = msbuild("10.0"),
  },
  msvc11 = {
    generator = "vs2012",
    build = msbuild("11.0"),
  },
  msvc12 = {
    generator = "vs2013",
    build = msbuild("12.0"),
  },
  -- disabled, likely permanently
  --[[ue4_2 = {
    generator = "vs2013",
    parameters = "--ue=4_2",
    build = uebuild("4.2"),
  },]]
  ue4_3 = {
    generator = "vs2013",
    parameters = "--ue=4_3",
    build = uebuild("4.3")
  },
  ue4_4 = {
    generator = "vs2013",
    parameters = "--ue=4_4",
    build = uebuild("4.4")
  },
  ue4_5 = {
    generator = "vs2013",
    parameters = "--ue=4_5",
    build = uebuild("4.5")
  },
  ue4_6 = {
    generator = "vs2013",
    parameters = "--ue=4_6",
    build = uebuild("4.6")
  },
  -- disabled, possibly permanently
  --[[mingw = {
    generator = "gmake",
    --path = "/cygdrive/c/mingw/bin",
    build = "PATH=/cygdrive/c/mingw/bin mingw32-make -j12 config=debug && PATH=/cygdrive/c/mingw/bin mingw32-make -j12 config=release && cp `which pthreadGC2.dll` ../../bin/mingw/test",
  },]]
}
