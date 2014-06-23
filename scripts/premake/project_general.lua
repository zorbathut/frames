
local name, src, include, projectInfo = ...

project(name)
  kind "StaticLib"
  language "C++"
  location(projectInfo.path)
  files(src)
  files(include)
  
  filter "architecture:x32"
    targetdir("lib/" .. projectInfo.slug .. "/x32")
      
  filter "architecture:x64"
    targetdir("lib/" .. projectInfo.slug .. "/x64")