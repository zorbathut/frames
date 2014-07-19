
using UnrealBuildTool;

public class plugin_ue4 : ModuleRules
{
  public plugin_ue4(TargetInfo Target)
  {
    PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
    PrivateDependencyModuleNames.AddRange(new string[] {  });
  }
}
