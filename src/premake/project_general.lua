
local name, src, include, projectInfo = ...

project(name)
  kind "StaticLib"
  language "C++"
  location(projectInfo.path)
  files(src)
  files(include)
  
  configuration "x32"
    targetdir("lib/" .. projectInfo.slug .. "/x32")
      
  configuration "x64"
    targetdir("lib/" .. projectInfo.slug .. "/x64")