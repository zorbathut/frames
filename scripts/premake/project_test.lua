local projectInfo = ...

project "test"
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
  
  if projectInfo.ue4_path then
    linkoptions {"/ignore:4099"}  -- ue4 libraries don't include pdb's
  end
  
  filter {"action:vs*", "configurations:Debug"}
    linkoptions {"/NODEFAULTLIB:MSVCRT"}
  
  filter "architecture:x32"
    targetdir("bin/" .. projectInfo.slug .. "/x32/test")
      
  filter "architecture:x64"
    targetdir("bin/" .. projectInfo.slug .. "/x64/test")
  
  -- need to mkdir first because msvc9 doesn't do it by default; doing it everywhere for consistency's sake
  filter {"action:vs*", "architecture:x32", "not action:vs2012"}
    prebuildcommands{[[mkdir $(OutDir)]], [[copy "c:\Program Files (x86)\Windows Kits\8.1\Redist\D3D\x86\D3DCompiler_47.dll" $(OutDir)]]}
    
  filter {"action:vs*", "architecture:x64", "not action:vs2012"}
    prebuildcommands{[[mkdir $(OutDir)]], [[copy "c:\Program Files (x86)\Windows Kits\8.1\Redist\D3D\x64\D3DCompiler_47.dll" $(OutDir)]]}
  
  filter {"action:vs2012", "architecture:x32"}
    prebuildcommands{[[mkdir $(OutDir)]], [[copy "c:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\x86_amd64\D3DCompiler_46.dll" $(OutDir)]]}
    
  filter {"action:vs2012", "architecture:x64"}
    prebuildcommands{[[mkdir $(OutDir)]], [[copy "c:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\Packages\Debugger\X64\D3DCompiler_46.dll" $(OutDir)]]}
    
  filter "action:vs2012"
    defines "_VARIADIC_MAX=10" -- MSVC11 has sketchy support for tr1::tuple; this is required for google test to work
  
  filter "configurations:Release"
    links {"gtest"}
    
  filter "configurations:Debug"
    links {"gtestd"}
    
  filter {}
    links {"frames", "frames_opengl", "frames_null", "SDL2", "winmm", "version", "imm32"}
  
    -- These should really be part of frames, but premake doesn't deal with them properly in that case
    links {"glew32s", "opengl32", "jpeg"}
    
  filter "action:vs*"
    links {"frames_dx11", "d3d11", "dxgi", "lua51", "d3dcompiler"}
    
    if not projectInfo.ue4_path then
      links {"freetype253MT", "libpng14", "zlib"}
    else
      links {"freetype2412MT"}
    end
  
  if projectInfo.ue4_path then
    filter {"action:vs*", "architecture:x32"}
      links {"libpng", "zlib"}
    
    filter {"action:vs*", "architecture:x64"}
      links {"libpng_64", "zlib_64"}
  end