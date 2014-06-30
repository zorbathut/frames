-- TODO: this is turning into kind of a giant blob of complexity. Someday this should be cleaned up, but it's not yet clear what lines it should be cleaned up along.

dofile("scripts/premake/util.lua")

newoption {
  trigger = "ue",
  value = "version",
  description = "Choose an Unreal Engine version to target; requires vs2013 target",
  allowed = {
    { "4_2", "Unreal Engine 4.2" },
  }
}

-- utility functions
function filereplace(filename, src, dst)
  local f = io.open(filename, "rb")
  local data = f:read("*all")
  f:close()
  
  data = data:gsub(src, dst)
  
  f = io.open(filename, "wb")
  f:write(data)
  f:close()
end
-- end utility functions

local projectInfo = {}

if _ACTION == "vs2008" then
  projectInfo.slug = "msvc9"
  projectInfo.platform = "win"
  projectInfo.platformFull = "win_msvc9_"
elseif _ACTION == "vs2010" then
  projectInfo.slug = "msvc10"
  projectInfo.platform = "win"
  projectInfo.platformFull = "win_msvc10_"
elseif _ACTION == "vs2012" then
  projectInfo.slug = "msvc11"
  projectInfo.platform = "win"
  projectInfo.platformFull = "win_msvc11_"
elseif _ACTION == "vs2013" and not _OPTIONS["ue"] then
  projectInfo.slug = "msvc12"
  projectInfo.platform = "win"
  projectInfo.platformFull = "win_msvc12_"
elseif _ACTION == "vs2013" and _OPTIONS["ue"] == "4_2" then
  projectInfo.slug = "ue4_2"
  projectInfo.platform = "win"
  projectInfo.platformFull = "win_msvc12_"
  projectInfo.ue4_path = "C:/Program Files/Unreal Engine/4.2/"
  os.execute([["/Program Files/Unreal Engine/Launcher/Engine/Binaries/Win64/UnrealVersionSelector.exe" /projectfiles %cd%/ue4/ue4.uproject]])
  filereplace("ue4/Intermediate/ProjectFiles/ue4.vcxproj", "$%(SolutionDir%)$%(SolutionName%)", "$(ProjectDir)/../../$(ProjectName)")
  filereplace("ue4/Intermediate/ProjectFiles/ue4.vcxproj.user", "$%(SolutionDir%)$%(SolutionName%)", "$(ProjectDir)/../../$(ProjectName)")
  filereplace("ue4/Intermediate/ProjectFiles/ue4.vcxproj.user", "</LocalDebuggerCommandArguments>", " -opengl</LocalDebuggerCommandArguments>") -- Run in opengl mode because that's the only renderer we support right now
else
  print(("Not supported: target %s with OS %s"):format(_ACTION or "", _OS or ""))
  projectInfo.slug = ""
  projectInfo.platform = ""
  projectInfo.platformFull = ""
end

projectInfo.path = "projects/" .. projectInfo.slug

solution "Frames"
  platforms { "x32", "x64" }
  configurations { "Debug", "Release" }
  
  flags { "FatalWarnings" }
  
  -- Paths
  location(projectInfo.path)
  
  local function libincludes(suffix)
    if not projectInfo.ue4_path then
      includedirs {
        "include",
        "deps/boost_1_55_0",
        "deps/freetype-2.5.3/" .. projectInfo.platform .. suffix .. "/include",
        "deps/glew-1.10.0/" .. projectInfo.platform .. suffix .. "/include",
        "deps/jpeg-9/" .. projectInfo.platform .. suffix .. "/include",
        "deps/libpng-1.4.3/" .. projectInfo.platform .. suffix .. "/include",
        "deps/lua-5.1.5/" .. projectInfo.platform .. suffix .. "/include",
        "deps/SDL2-2.0.1/" .. projectInfo.platform .. suffix .. "/include",
        "deps/zlib-1.2.8/" .. projectInfo.platform .. suffix .. "/include",
        "deps/gtest-1.7.0/" .. projectInfo.platformFull .. suffix .. "/include",
      }
      libdirs {
        "deps/boost_1_55_0",
        "deps/freetype-2.5.3/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/glew-1.10.0/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/jpeg-9/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/libpng-1.4.3/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/lua-5.1.5/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/SDL2-2.0.1/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/zlib-1.2.8/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/gtest-1.7.0/" .. projectInfo.platformFull .. suffix .. "/lib",
      }
    else
      local ue4libsuffix = "Win" .. suffix .. "/VS2013"
      includedirs {
        "include",
        "deps/boost_1_55_0",
        projectInfo.ue4_path .. "/Engine/Source/ThirdParty/FreeType2/FreeType2-2.4.12/include",
        "deps/glew-1.10.0/" .. projectInfo.platform .. suffix .. "/include",
        "deps/jpeg-9/" .. projectInfo.platform .. suffix .. "/include",
        projectInfo.ue4_path .. "/Engine/Source/ThirdParty/libPNG/libPNG-1.5.2",
        "deps/lua-5.1.5/" .. projectInfo.platform .. suffix .. "/include",
        "deps/SDL2-2.0.1/" .. projectInfo.platform .. suffix .. "/include",
        projectInfo.ue4_path .. "/Engine/Source/ThirdParty/zlib/zlib-1.2.5/Inc",
        "deps/gtest-1.7.0/" .. projectInfo.platformFull .. suffix .. "/include",
      }
      libdirs {
        "deps/boost_1_55_0",
        projectInfo.ue4_path .. "/Engine/Source/ThirdParty/FreeType2/FreeType2-2.4.12/Lib/" .. ue4libsuffix,
        "deps/glew-1.10.0/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/jpeg-9/" .. projectInfo.platform .. suffix .. "/lib",
        projectInfo.ue4_path .. "/Engine/Source/ThirdParty/libPNG/libPNG-1.5.2/lib/" .. ue4libsuffix,
        "deps/lua-5.1.5/" .. projectInfo.platform .. suffix .. "/lib",
        "deps/SDL2-2.0.1/" .. projectInfo.platform .. suffix .. "/lib",
        projectInfo.ue4_path .. "/Engine/Source/ThirdParty/zlib/zlib-1.2.5/Lib/Win" .. suffix,
        "deps/gtest-1.7.0/" .. projectInfo.platformFull .. suffix .. "/lib",
      }
    end
  end
  
  if projectInfo.platform == "win" then
    defines "NOMINMAX"  -- disable min/max macros from windows headers
    defines "WIN32_LEAN_AND_MEAN"  -- make windows.h not quite as titanically huge
  end
  
  flags { "Symbols" } -- always create debug symbols
  
  -- architecture-specific tweaks
  filter "architecture:x32"
    libincludes("32")
  
  filter "architecture:x64"
    libincludes("64")
  
  -- Compiler-specific tweaks
  filter "action:vs*"
    defines "_CRT_SECURE_NO_WARNINGS" -- Annoying warning on MSVC that wants use of MSVC-specific functions
    buildoptions "/wd4800"  -- "forcing value to bool 'true' or 'false' (performance warning)"
  
  filter "action:vs2008 or action:vs2010"
    includedirs {
      "C:/Program Files (x86)/Windows Kits/8.1/Include/um",
      "C:/Program Files (x86)/Windows Kits/8.1/Include/shared"
    }
      
  filter { "action:vs2008 or action:vs2010", "architecture:x32" }
    libdirs "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86"
    
  filter { "action:vs2008 or action:vs2010", "architecture:x64" }
    libdirs "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x64"
  
  -- Build config
  filter "configurations:Debug"
    targetsuffix "d"

  filter "configurations:Release"
    optimize "Full"
  
  -- UE4 project; do this first if available so it's the default project  
  if projectInfo.ue4_path then
    dofile("scripts/premake/project_ue4.lua", projectInfo)
  end

  -- Test project; alternate default project
  dofile("scripts/premake/project_test.lua", projectInfo)
  
  -- Main project
  dofile("scripts/premake/project_core.lua", projectInfo)
  
  -- Renderers
  dofile("scripts/premake/project_opengl.lua", projectInfo)
  dofile("scripts/premake/project_dx11.lua", projectInfo)
  dofile("scripts/premake/project_null.lua", projectInfo)
