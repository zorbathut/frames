
-- TODO: this is turning into kind of a giant blob of complexity. Someday this should be cleaned up, but it's not yet clear what lines it should be cleaned up along.

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

local slug
local platform
local platformFull

local uepath

if _ACTION == "vs2008" then
  slug = "msvc9"
  platform = "win"
  platformFull = "win_msvc9_"
elseif _ACTION == "vs2010" then
  slug = "msvc10"
  platform = "win"
  platformFull = "win_msvc10_"
elseif _ACTION == "vs2012" then
  slug = "msvc11"
  platform = "win"
  platformFull = "win_msvc11_"
elseif _ACTION == "vs2013" and not _OPTIONS["ue"] then
  slug = "msvc12"
  platform = "win"
  platformFull = "win_msvc12_"
elseif _ACTION == "vs2013" and _OPTIONS["ue"] == "4_2" then
  slug = "ue4_2"
  platform = "win"
  platformFull = "win_msvc12_"
  uepath = "C:/Program Files/Unreal Engine/4.2/"
  os.execute([["/Program Files/Unreal Engine/Launcher/Engine/Binaries/Win64/UnrealVersionSelector.exe" /projectfiles %cd%/ue4/ue4.uproject]])
  filereplace("ue4/Intermediate/ProjectFiles/ue4.vcxproj", "$%(SolutionDir%)$%(SolutionName%)", "$(ProjectDir)/../../$(ProjectName)")
else
  print(("Not supported: target %s with OS %s"):format(_ACTION or "", _OS or ""))
  slug = ""
  platform = ""
  platformFull = ""
end

local path = "projects/" .. slug

solution "Frames"
  configurations { "Debug", "Release" }
  if not uepath then
    platforms { "x32", "x64" }
  else
    platforms { "x64" } -- currently editor only, which means x64 only
  end
  
  flags { "FatalWarnings" }
  
  -- Paths
  location(path)
  
  local function libincludes(suffix)
    if not uepath then
      includedirs {
        "include",
        "deps/boost_1_55_0",
        "deps/freetype-2.5.3/" .. platform .. suffix .. "/include",
        "deps/glew-1.10.0/" .. platform .. suffix .. "/include",
        "deps/jpeg-9/" .. platform .. suffix .. "/include",
        "deps/libpng-1.4.3/" .. platform .. suffix .. "/include",
        "deps/lua-5.1.5/" .. platform .. suffix .. "/include",
        "deps/SDL2-2.0.1/" .. platform .. suffix .. "/include",
        "deps/zlib-1.2.8/" .. platform .. suffix .. "/include",
        "deps/gtest-1.7.0/" .. platformFull .. suffix .. "/include",
      }
      libdirs {
        "deps/boost_1_55_0",
        "deps/freetype-2.5.3/" .. platform .. suffix .. "/lib",
        "deps/glew-1.10.0/" .. platform .. suffix .. "/lib",
        "deps/jpeg-9/" .. platform .. suffix .. "/lib",
        "deps/libpng-1.4.3/" .. platform .. suffix .. "/lib",
        "deps/lua-5.1.5/" .. platform .. suffix .. "/lib",
        "deps/SDL2-2.0.1/" .. platform .. suffix .. "/lib",
        "deps/zlib-1.2.8/" .. platform .. suffix .. "/lib",
        "deps/gtest-1.7.0/" .. platformFull .. suffix .. "/lib",
      }
    else
      local ue4libsuffix = "Win" .. suffix .. "/VS2013"
      includedirs {
        "include",
        "deps/boost_1_55_0",
        uepath .. "/Engine/Source/ThirdParty/FreeType2/FreeType2-2.4.12/include",
        "deps/glew-1.10.0/" .. platform .. suffix .. "/include",
        "deps/jpeg-9/" .. platform .. suffix .. "/include",
        uepath .. "/Engine/Source/ThirdParty/libPNG/libPNG-1.5.2",
        "deps/lua-5.1.5/" .. platform .. suffix .. "/include",
        "deps/SDL2-2.0.1/" .. platform .. suffix .. "/include",
        uepath .. "/Engine/Source/ThirdParty/zlib/zlib-1.2.5/Inc",
        "deps/gtest-1.7.0/" .. platformFull .. suffix .. "/include",
      }
      libdirs {
        "deps/boost_1_55_0",
        uepath .. "/Engine/Source/ThirdParty/FreeType2/FreeType2-2.4.12/Lib/" .. ue4libsuffix,
        "deps/glew-1.10.0/" .. platform .. suffix .. "/lib",
        "deps/jpeg-9/" .. platform .. suffix .. "/lib",
        uepath .. "/Engine/Source/ThirdParty/libPNG/libPNG-1.5.2/lib/" .. ue4libsuffix,
        "deps/lua-5.1.5/" .. platform .. suffix .. "/lib",
        "deps/SDL2-2.0.1/" .. platform .. suffix .. "/lib",
        uepath .. "/Engine/Source/ThirdParty/zlib/zlib-1.2.5/Lib/Win" .. suffix,
        "deps/gtest-1.7.0/" .. platformFull .. suffix .. "/lib",
      }
    end
  end
  
  if platform == "win" then
    defines "NOMINMAX"  -- disable min/max macros from windows headers
    defines "WIN32_LEAN_AND_MEAN"  -- make windows.h not quite as titanically huge
  end
  
  -- Platform-specific tweaks
  configuration "x32"
    libincludes("32")
  
  configuration "x64"
    libincludes("64")
  
  -- Compiler-specific tweaks
  configuration "vs*"
    defines "_CRT_SECURE_NO_WARNINGS" -- Annoying warning on MSVC that wants use of MSVC-specific functions
    buildoptions "/wd4800"  -- "forcing value to bool 'true' or 'false' (performance warning)"
  
  configuration { "vs2008 or vs2010" }
    includedirs {
      "C:/Program Files (x86)/Windows Kits/8.1/Include/um",
      "C:/Program Files (x86)/Windows Kits/8.1/Include/shared"
    }
      
  configuration { "vs2008 or vs2010", "x32" }
    libdirs "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86"
    
  configuration { "vs2008 or vs2010", "x64" }
    libdirs "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x64"
  
  -- Build config
  configuration "*"
    flags { "Symbols" } -- always create debug symbols
  
  configuration "Debug"
    targetsuffix "d"

  configuration "Release"
    optimize "Full"

  local function linkWithFrames()
    configuration {}
    
    links {"frames", "frames_opengl", "frames_null", "SDL2", "winmm", "version", "imm32"}
  
    -- These should really be part of frames, but premake doesn't deal with them properly in that case
    links {"glew32s", "opengl32", "jpeg"}
    
    configuration "vs*"
      links {"frames_dx11", "d3d11", "dxgi", "lua51", "d3dcompiler"}
      
      if not uepath then
        links {"freetype253MT", "libpng14", "zlib"}
      else
        links {"freetype2412MT"}
      end
    
    if uepath then
      configuration {"vs*", "x32"}
        links {"libpng", "zlib"}
      
      configuration {"vs*", "x64"}
        links {"libpng_64", "zlib_64"}
    end
      
    configuration {}
  end
  
  -- Frames
  project "frames"
    kind "StaticLib"
    language "C++"
    location(path)
    files "src/core/*.cpp"
    files "include/frames/*.h"
    
    configuration "x32"
      targetdir("lib/" .. slug .. "/x32")
        
    configuration "x64"
      targetdir("lib/" .. slug .. "/x64")
  
  -- OGL core
  project "frames_opengl"
    kind "StaticLib"
    language "C++"
    location(path)
    files "src/opengl/*.cpp"
    files "include/frames/renderer_opengl.h"
    
    configuration "x32"
      targetdir("lib/" .. slug .. "/x32")
        
    configuration "x64"
      targetdir("lib/" .. slug .. "/x64")
  
  -- DX11 core
  project "frames_dx11"
    kind "StaticLib"
    language "C++"
    location(path)
    files "src/dx11/*.cpp"
    files "include/frames/renderer_dx11.h"
    
    configuration "x32"
      targetdir("lib/" .. slug .. "/x32")
        
    configuration "x64"
      targetdir("lib/" .. slug .. "/x64")
  
  if uepath then
    external "ue4"
      location "ue4/Intermediate/ProjectFiles"
      kind "StaticLib"
      
      -- this will have to be fixed, but I don't yet grok the ue4 configuration options
      configmap {
        ["Debug"] = {"Development_Editor", "x64"},
        ["Release"] = {"Development_Editor", "x64"},
      }

      links "frames"
  end
  
  -- Null core
  project "frames_null"
    kind "StaticLib"
    language "C++"
    location(path)
    files "src/null/*.cpp"
    files "include/frames/renderer_null.h"
    
    configuration "x32"
      targetdir("lib/" .. slug .. "/x32")
        
    configuration "x64"
      targetdir("lib/" .. slug .. "/x64")
      
  -- Test
  project "test"
    kind "ConsoleApp"
    language "C++"
    location(path)
    files "test/*.cpp"
    files "test/*.h"
    files "test/doc/*.cpp"
    files "test/doc/*.h"
    includedirs "test"
    debugdir "test"
    
    linkoptions {"/NODEFAULTLIB:LIBCMT"}
    
    if uepath then
      linkoptions {"/ignore:4099"}  -- ue4 libraries don't include pdb's
    end
    
    configuration {"vs*", "Debug"}
      linkoptions {"/NODEFAULTLIB:MSVCRT"}
    
    configuration "x32"
      targetdir("bin/" .. slug .. "/x32/test")
        
    configuration "x64"
      targetdir("bin/" .. slug .. "/x64/test")
    
    -- need to mkdir first because msvc9 doesn't do it by default; doing it everywhere for consistency's sake
    configuration {"vs*", "x32", "not vs2012"}
      prebuildcommands{[[mkdir $(OutDir)]], [[copy "c:\Program Files (x86)\Windows Kits\8.1\Redist\D3D\x86\D3DCompiler_47.dll" $(OutDir)]]}
      
    configuration {"vs*", "x64", "not vs2012"}
      prebuildcommands{[[mkdir $(OutDir)]], [[copy "c:\Program Files (x86)\Windows Kits\8.1\Redist\D3D\x64\D3DCompiler_47.dll" $(OutDir)]]}
    
    configuration {"vs2012", "x32"}
      prebuildcommands{[[mkdir $(OutDir)]], [[copy "c:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\x86_amd64\D3DCompiler_46.dll" $(OutDir)]]}
      
    configuration {"vs2012", "x64"}
      prebuildcommands{[[mkdir $(OutDir)]], [[copy "c:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\Packages\Debugger\X64\D3DCompiler_46.dll" $(OutDir)]]}
      
    configuration "vs2012"
      defines "_VARIADIC_MAX=10" -- MSVC11 has sketchy support for tr1::tuple; this is required for google test to work
    
    configuration "Release"
      links {"gtest"}
      
    configuration "Debug"
      links {"gtestd"}
      
    linkWithFrames() -- must be after gtest because of the braindamaged way gcc links with libraries

  --[[project "win32_ogl"
    kind "WindowedApp"
    language "C++"
    location(path)
    files "samples/bootstrap/win32_ogl.cpp"
    debugdir "samples/bootstrap"
    flags "WinMain"
    
    configuration "x32"
      targetdir("bin/" .. slug .. "/x32/samples")
        
    configuration "x64"
      targetdir("bin/" .. slug .. "/x64/samples")

    linkWithFrames()]]