/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

using UnrealBuildTool;

using System.IO;

public class Frames : ModuleRules
{
  public Frames(TargetInfo Target)
  {
    PublicDependencyModuleNames.AddRange(new string[] {
      "Core", // needed for many many things
      "CoreUObject",  // for UObject
      //"InputCore",  // almost certain to be re-added
      "Engine", // for AHUD
      "RenderCore", // render queueing
      "ShaderCore", // shader machinery
      "RHI", // for actual rendering
    });
    
    string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x32";
    string PlatformStringFrames = (Target.Platform == UnrealTargetPlatform.Win64) ? "win64" : "win32";
    string DirectoryName = Path.GetDirectoryName( RulesCompiler.GetModuleFilename( this.GetType().Name ) );
    string FramesBase = Path.Combine( DirectoryName, "..", "..", "..", "..", ".." );
    
    // choose appropriate libraries
    if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
    {
      PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "framesd.lib"));
      PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames_renderer_nulld.lib"));
      PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames_renderer_dx11d.lib"));
      PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames_renderer_opengld.lib"));
    }
    else
    {
      PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames.lib"));
      PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames_renderer_null.lib"));
      PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames_renderer_dx11.lib"));
      PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "lib", "ue4_2", PlatformString, "frames_renderer_opengl.lib"));
    }
    
    PublicAdditionalLibraries.Add(Path.Combine(FramesBase, "deps", "jpeg-9", PlatformStringFrames, "lib", "jpeg.lib"));
    PublicAdditionalLibraries.Add("d3dcompiler.lib");
    PublicDependencyModuleNames.AddRange(new string[] { "FreeType2", "UElibPNG", "zlib" });

    PublicIncludePaths.Add(Path.Combine(FramesBase, "include"));
    PublicIncludePaths.Add(Path.Combine(FramesBase, "deps", "boost_1_55_0"));
  }
}
