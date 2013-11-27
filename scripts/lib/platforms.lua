
projects = {
  msvc9 = {
    generator = "vs2008",
    build = [[tmp= temp= cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=Release /m]],
  },
  msvc10 = {
    generator = "vs2010",
    build = [[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=Release /m]],
  },
  msvc11 = {
    generator = "vs2012",
    build = [[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 11.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=Release /m]],
  },
  msvc12 = {
    generator = "vs2013",
    build = [[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 12.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=Release /m]],
  },
  mingw = {
    generator = "gmake",
    --path = "/cygdrive/c/mingw/bin",
    build = "PATH=/cygdrive/c/mingw/bin mingw32-make -j12",
  },
}
