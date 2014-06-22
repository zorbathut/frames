local projectInfo = ...

project "test"
  configurations { "Debug", "Release" }
  platforms { "x32", "x64" }
  kind "ConsoleApp"
  language "C++"
  location(projectInfo.path)
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
    targetdir("bin/" .. projectInfo.slug .. "/x32/test")
      
  configuration "x64"
    targetdir("bin/" .. projectInfo.slug .. "/x64/test")
  
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
    
  configuration {}
    links {"frames", "frames_opengl", "frames_null", "SDL2", "winmm", "version", "imm32"}
  
    -- These should really be part of frames, but premake doesn't deal with them properly in that case
    links {"glew32s", "opengl32", "jpeg"}
    
    configuration "vs*"
      links {"frames_dx11", "d3d11", "dxgi", "lua51", "d3dcompiler"}
      
      if not projectInfo.ue4_path then
        links {"freetype253MT", "libpng14", "zlib"}
      else
        links {"freetype2412MT"}
      end
    
    if projectInfo.ue4_path then
      configuration {"vs*", "x32"}
        links {"libpng", "zlib"}
      
      configuration {"vs*", "x64"}
        links {"libpng_64", "zlib_64"}
    end