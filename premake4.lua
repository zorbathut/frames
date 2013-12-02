
local slug
local platform
local platformFull

if _ACTION == "vs2008" then
  slug = "msvc9"
  platform = "win32"
  platformFull = "win32_9"
elseif _ACTION == "vs2010" then
  slug = "msvc10"
  platform = "win32"
  platformFull = "win32_10"
elseif _ACTION == "vs2012" then
  slug = "msvc11"
  platform = "win32"
  platformFull = "win32_11"
elseif _ACTION == "vs2013" then -- NYI
  slug = "msvc12"
  platform = "win32"
  platformFull = "win32_12"
elseif _ACTION == "gmake" and _OS == "windows" then
  slug = "mingw"
  platform = "mingw"
  platformFull = "mingw"
else
  print(("Not supported: target %s with OS %s"):format(_ACTION, _OS))
  assert(false)
end

local path = "projects/" .. slug

solution "Frames"
  configurations { "Debug", "Release" }
  
  -- Paths
  location(path)
  includedirs {
    "include",
    "deps/boost_1_54_0",
    "deps/freetype-2.3.12/" .. platform .. "/include",
    "deps/glew-1.10.0/" .. platform .. "/include",
    "deps/jpeg-9/" .. platform .. "/include",
    "deps/libpng-1.4.3/" .. platform .. "/include",
    "deps/lua-5.1.5/" .. platform .. "/include",
    "deps/SDL2-2.0.1/" .. platform .. "/include",
    "deps/zlib-1.2.8/" .. platform .. "/include",
    "deps/gtest-1.7.0/" .. platformFull .. "/include",
  }
  libdirs {
    "deps/boost_1_54_0",
    "deps/freetype-2.3.12/" .. platform .. "/lib",
    "deps/glew-1.10.0/" .. platform .. "/lib",
    "deps/jpeg-9/" .. platform .. "/lib",
    "deps/libpng-1.4.3/" .. platform .. "/lib",
    "deps/lua-5.1.5/" .. platform .. "/lib",
    "deps/SDL2-2.0.1/" .. platform .. "/lib",
    "deps/zlib-1.2.8/" .. platform .. "/lib",
    "deps/gtest-1.7.0/" .. platformFull .. "/lib",
  }
  
  -- Platform-specific tweaks
  configuration "vs*"
    defines "_CRT_SECURE_NO_WARNINGS" -- Annoying warning on MSVC that wants use of MSVC-specific functions
  
  configuration {"gmake" and "windows"}
    defines "_WIN32" -- Doesn't seem to be provided by default on cygwin
  
  -- Build config
  configuration "*"
    flags { "Symbols" } -- always create debug symbols
  
  configuration "Debug"
    targetsuffix "d"

  configuration "Release"
    flags { "Optimize" }  

  local function linkWithFrames()
    configuration {}
    
    links {"frames", "SDL2", "winmm", "version", "imm32"}
  
    -- These should really be part of frames, but premake doesn't deal with them properly in that case
    links {"glew32s", "opengl32", "jpeg"}
    
    configuration "vs*"
      links {"libpng14", "lua51", "freetype2312", "zlib"}
      
    configuration "gmake"
      links {"png14", "lua", "freetype", "z", "pthread", "gdi32", "ole32", "oleaut32", "uuid"}
      
    configuration {}
  end
  
  -- Frames
  project "frames"
    kind "StaticLib"
    language "C++"
    location(path)
    targetdir("lib/" .. slug)
    files "src/*.cpp"
    files "include/frames/*.h"

  -- Test
  project "test"
    kind "ConsoleApp"
    language "C++"
    location(path)
    targetdir("bin/" .. slug .. "/test")
    files "test/*.cpp"
    
    configuration {"vs2008", "Debug"}
      linkoptions "/NODEFAULTLIB:msvcrt" -- I think this is a result of not having special debug builds for everything. Might be fixed when those are added.
      
    configuration "vs2012"
      defines "_VARIADIC_MAX=10" -- MSVC11 has sketchy support for tr1::tuple; this is required for google test to work
    
    configuration {"Debug", "vs*"}
      links {"gtestd", "gtest_maind"}
      
    configuration {"Debug", "not vs*"}
      links {"gtest", "gtest_main"}
    
    configuration "Release"
      links {"gtest", "gtest_main"}
      
    linkWithFrames() -- must be after gtest because of the braindamaged way gcc links with libraries

  project "win32_ogl"
    kind "WindowedApp"
    language "C++"
    location(path)
    targetdir("bin/" .. slug .. "/samples")
    files "samples/bootstrap/win32_ogl.cpp"
    flags "WinMain"

    linkWithFrames()
