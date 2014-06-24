
local projectInfo = ...

external "ue4"
  location "ue4/Intermediate/ProjectFiles"
  kind "StaticLib"
  removeconfigurations { "Debug" }
  
  configmap { -- these configs should be mapped, one way or another, but right now there's no mapping that makes sense
    --[{"Debug", "x32"}] = {"Shipping_RocketGame", "x32"},
    [{"Release", "x32"}] = {"Shipping_RocketGame", "x32"},
    --[{"Debug", "x64"}] = {"Development_Editor", "x64"},
    [{"Release", "x64"}] = {"Development_Editor", "x64"},
  }

  links {"frames", "frames_null"}