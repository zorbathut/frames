
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
  if platform == "win32" then
    defines { "_CRT_SECURE_NO_WARNINGS" } -- Annoying warning on MSVC that wants use of MSVC-specific functions
  elseif platform == "mingw" then
    defines { "_WIN32" } -- Doesn't seem to be provided by default on cygwin
  end
  
  if slug == "msvc11" then
    defines { "_VARIADIC_MAX=10" } -- MSVC11 has sketchy support for tr1::tuple; this is required for google test to work
  end
  
  local function linkWithFrames()
    links {"frames", "SDL2", "winmm", "version", "imm32"}
  
    -- These should really be part of frames, but premake doesn't deal with them properly in that case
    links {"glew32s", "opengl32", "jpeg"}
    if platform == "win32" then
      links {"libpng14", "lua51", "freetype2312", "zlib"}
    elseif platform == "mingw" then
      links {"png14", "lua", "freetype", "z", "pthread", "gdi32", "ole32", "oleaut32", "uuid"}
    end
  end
  
  -- Build config
  flags { "Symbols" } -- always create debug symbols

  configuration "Release"
    flags { "Optimize" }  

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
    
    linkWithFrames()
    
    configuration "Debug"
      links {"gtestd", "gtest_maind"}
    
    configuration "Release"
      links {"gtest", "gtest_main"}

  project "win32_ogl"
    kind "WindowedApp"
    language "C++"
    location(path)
    targetdir("bin/" .. slug .. "/samples")
    files "samples/bootstrap/win32_ogl.cpp"
    flags "WinMain"

    linkWithFrames()
