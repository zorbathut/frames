
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
elseif _ACTION == "vs2013" then -- NYI
  slug = "msvc12"
  platform = "win"
  platformFull = "win_msvc12_"
else
  print(("Not supported: target %s with OS %s"):format(_ACTION, _OS))
  assert(false)
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
      "deps/freetype-2.3.12/" .. platform .. suffix .. "/include",
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
      "deps/freetype-2.3.12/" .. platform .. suffix .. "/lib",
      "deps/glew-1.10.0/" .. platform .. suffix .. "/lib",
      "deps/jpeg-9/" .. platform .. suffix .. "/lib",
      "deps/libpng-1.4.3/" .. platform .. suffix .. "/lib",
      "deps/lua-5.1.5/" .. platform .. suffix .. "/lib",
      "deps/SDL2-2.0.1/" .. platform .. suffix .. "/lib",
      "deps/zlib-1.2.8/" .. platform .. suffix .. "/lib",
      "deps/gtest-1.7.0/" .. platformFull .. suffix .. "/lib",
    }
  end
  
  -- Platform-specific tweaks
  configuration "x32"
    libincludes("32")
  
  configuration "x64"
    libincludes("64")
  
  -- Compiler-specific tweaks
  configuration "vs*"
    defines "_CRT_SECURE_NO_WARNINGS" -- Annoying warning on MSVC that wants use of MSVC-specific functions
  
  configuration { "vs2010", "x64" }
    libdirs "C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0a\\Lib\\x64"  -- why on earth is this not included by default
  
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
      links {"libpng14", "lua51", "freetype2312MT", "zlib"}
      
    configuration {}
  end
  
  -- Frames
  project "frames"
    kind "StaticLib"
    language "C++"
    location(path)
    files "src/*.cpp"
    files "include/frames/*.h"
    
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
      
    configuration "vs2012"
      defines "_VARIADIC_MAX=10" -- MSVC11 has sketchy support for tr1::tuple; this is required for google test to work
    
    configuration "Release"
      links {"gtest", "gtest_main"}
      
    configuration "Debug"
      links {"gtestd", "gtest_maind"}
      
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
