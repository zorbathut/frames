
using UnrealBuildTool;

public class ue4 : ModuleRules
{
  public ue4(TargetInfo Target)
  {
    PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
    PrivateDependencyModuleNames.AddRange(new string[] {  });
  }
}
