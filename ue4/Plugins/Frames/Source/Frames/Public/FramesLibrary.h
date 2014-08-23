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

  UFUNCTION(BlueprintCallable, Category="Frames", meta=(HidePin="WorldContextObject", DefaultToSelf="WorldContextObject"))
  static UFramesConfigurationLocal *FramesConfigurationLocalCreate(UObject* WorldContextObject);
  
  UFUNCTION(BlueprintCallable, Category="Frames", meta=(HidePin="WorldContextObject", DefaultToSelf="WorldContextObject"))
  static UFramesEnvironment *FramesEnvironmentCreate(UObject* WorldContextObject, UFramesConfigurationLocal *Configuration);

  UFUNCTION(BlueprintCallable, Category="Frames|Creation", meta=(HidePin="WorldContextObject", DefaultToSelf="WorldContextObject"))
  static UFramesFrame *FramesFrameCreate(UObject* WorldContextObject, UFramesLayout *Parent, const FString &Name);

  UFUNCTION(BlueprintCallable, Category="Frames|Creation", meta=(HidePin="WorldContextObject", DefaultToSelf="WorldContextObject"))
  static UFramesMask *FramesMaskCreate(UObject* WorldContextObject, UFramesLayout *Parent, const FString &Name);

  UFUNCTION(BlueprintCallable, Category="Frames|Creation", meta=(HidePin="WorldContextObject", DefaultToSelf="WorldContextObject"))
  static UFramesSprite *FramesSpriteCreate(UObject* WorldContextObject, UFramesLayout *Parent, const FString &Name);

  UFUNCTION(BlueprintCallable, Category="Frames|Creation", meta=(HidePin="WorldContextObject", DefaultToSelf="WorldContextObject"))
  static UFramesText *FramesTextCreate(UObject* WorldContextObject, UFramesLayout *Parent, const FString &Name);
};
