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
    along with Frames.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "FramesConfigurationLocal.h"
#include "FramesEnvironment.h"
#include "FramesMask.h"
#include "FramesSprite.h"
#include "FramesText.h"

#include "FramesLibrary.generated.h"

UCLASS()
class FRAMES_API UFramesLibrary : public UBlueprintFunctionLibrary {
  GENERATED_UCLASS_BODY()

  UFUNCTION(BlueprintCallable, Category="Frames")
  static UFramesConfigurationLocal *FramesConfigurationLocalCreate();
  
  UFUNCTION(BlueprintCallable, Category = "Frames")
  static UFramesEnvironment *FramesEnvironmentCreate(UFramesConfigurationLocal *Configuration);

  UFUNCTION(BlueprintCallable, Category = "Frames|Creation")
  static UFramesFrame *FramesFrameCreate(UFramesLayout *Parent, const FString &Name);

  UFUNCTION(BlueprintCallable, Category = "Frames|Creation")
  static UFramesMask *FramesMaskCreate(UFramesLayout *Parent, const FString &Name);

  UFUNCTION(BlueprintCallable, Category = "Frames|Creation")
  static UFramesSprite *FramesSpriteCreate(UFramesLayout *Parent, const FString &Name);

  UFUNCTION(BlueprintCallable, Category = "Frames|Creation")
  static UFramesText *FramesTextCreate(UFramesLayout *Parent, const FString &Name);
};
