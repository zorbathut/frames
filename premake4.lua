
newoption {
  trigger = "ue",
  value = "version",
  description = "Choose an Unreal Engine version to target",
  allowed = {
    { "4_2", "Unreal Engine 4.2" },
  }
}

local slug
local platform
local platformFull

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
else
  print(("Not supported: target %s with OS %s"):format(_ACTION or "", _OS or ""))
  slug = ""
  platform = ""
  platformFull = ""
end

local path = "projects/" .. slug

solution "Frames"
  configurations { "Debug", "Release" }
  platforms { "x32", "x64" }
  
  flags { "FatalWarnings" }
  
  -- Paths
  location(path)
  
  local function libincludes(suffix)
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
    flags { "Optimize" }  

  local function linkWithFrames()
    configuration {}
    
    links {"frames", "frames_opengl", "frames_null", "SDL2", "winmm", "version", "imm32"}
  
    -- These should really be part of frames, but premake doesn't deal with them properly in that case
    links {"glew32s", "opengl32", "jpeg"}
    
    configuration "vs*"
      links {"frames_dx11", "d3d11", "dxgi", "libpng14", "lua51", "freetype253MT", "zlib", "d3dcompiler"}
      
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

  project "win32_ogl"
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

    linkWithFrames()
