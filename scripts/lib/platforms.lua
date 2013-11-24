
projects = {
  msvc9 = {
    generator = "Visual Studio 9 2008",
    build = [[tmp= temp= cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 9.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=Release]],
  },
  msvc10 = {
    generator = "Visual Studio 10",
    build = [[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 10.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=Release]],
  },
  msvc11 = {
    generator = "Visual Studio 11",
    build = [[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 11.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=Release]],
  },
  msvc12 = {
    generator = "Visual Studio 12",
    build = [[cmd /Q /C call "c:/Program Files (x86)/Microsoft Visual Studio 12.0/Common7/Tools/vsvars32.bat" "&&" msbuild Frames.sln /p:configuration=Release]],
  },
  mingw = {
    generator = "MinGW Makefiles",
    path = "/cygdrive/c/mingw/bin",
    build = "mingw32-make",
  },
}
