
using UnrealBuildTool;

using System.IO;

public class test : ModuleRules
{
	public test(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
    
    // FRAMES_SPECIFIC INIT UNDER HERE
    
    string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
    string PlatformStringFrames = (Target.Platform == UnrealTargetPlatform.Win64) ? "win64" : "win32";
    string DirectoryName = Path.GetDirectoryName( RulesCompiler.GetModuleFilename( this.GetType().Name ) );
    string FramesBase = Path.Combine( DirectoryName, "..", "..", "..", ".." );
    
    PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames.lib"));
    PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames_dx11.lib"));
    
    PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "deps", "jpeg-9", PlatformStringFrames, "lib", "jpeg.lib"));
    PublicAdditionalLibraries.Add("d3dcompiler.lib");
    PublicDependencyModuleNames.AddRange(new string[] { "FreeType2", "UElibPNG", "zlib" });

    PublicIncludePaths.Add(Path.Combine(FramesBase, "include"));
    PublicIncludePaths.Add(Path.Combine(FramesBase, "deps", "boost_1_55_0"));
	}
}
