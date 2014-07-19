

using UnrealBuildTool;
using System.Collections.Generic;

public class plugin_ue4EditorTarget : TargetRules
{
	public plugin_ue4EditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "plugin_ue4" } );
	}
}
