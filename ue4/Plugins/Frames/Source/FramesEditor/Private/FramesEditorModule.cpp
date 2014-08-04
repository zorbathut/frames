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

#include "FramesEditorPCH.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_Base.h"
#include "FramesFont.h"

class FAssetTypeActions_FramesFont : public FAssetTypeActions_Base
{
public:
  // IAssetTypeActions Implementation
  virtual FText GetName() const override { return FText::FromString(TEXT("Frames Font")); }
  virtual FColor GetTypeColor() const override { return FColor(255,0,0); }
  virtual UClass*GetSupportedClass() const override { return UFramesFont::StaticClass(); }
  virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }
};

class FFramesEditorModule : public IModuleInterface {
public:
  virtual void StartupModule() override {
    // Register asset types
    IAssetTools &assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    fontTypeActions = MakeShareable(new FAssetTypeActions_FramesFont);
    assetTools.RegisterAssetTypeActions(fontTypeActions.ToSharedRef());
  }

  virtual void ShutdownModule() override {
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) {
      return;
    }

    IAssetTools &assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(fontTypeActions.ToSharedRef());
  }

private:
  TSharedPtr<IAssetTypeActions> fontTypeActions;
};

IMPLEMENT_MODULE(FFramesEditorModule, FramesEditor);
