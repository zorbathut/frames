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

#include <frames/configuration.h>
#include <frames/layout.h>

#include "boost/static_assert.hpp"

class UFramesFrame;
class UFramesEnvironment;

#include "FramesRect.h"

#include "FramesLayout.generated.h"

UENUM()
enum EFramesInputMode {
	FIM_NONE UMETA(DisplayName="None"),
	FIM_ALL UMETA(DisplayName="All"),
};

UENUM()
enum EFramesAnchor {
  EFA_TOPLEFT UMETA(DisplayName="TopLeft"),
  EFA_TOPCENTER UMETA(DisplayName="TopCenter"),
  EFA_TOPRIGHT UMETA(DisplayName="TopRight"),
  EFA_CENTERLEFT UMETA(DisplayName="CenterLeft"),
  EFA_CENTER UMETA(DisplayName="Center"),
  EFA_CENTERRIGHT UMETA(DisplayName="CenterRight"),
  EFA_BOTTOMLEFT UMETA(DisplayName="BottomLeft"),
  EFA_BOTTOMCENTER UMETA(DisplayName="BottomCenter"),
  EFA_BOTTOMRIGHT UMETA(DisplayName="BottomRight"),
  EFA_LEFT UMETA(DisplayName="Left"),
  EFA_CENTERX UMETA(DisplayName="Centerx"),
  EFA_RIGHT UMETA(DisplayName="Right"),
  EFA_TOP UMETA(DisplayName="Top"),
  EFA_CENTERY UMETA(DisplayName="Centery"),
  EFA_BOTTOM UMETA(DisplayName="Bottom"),
};

UENUM()
enum EFramesAxis {
  EFX_X UMETA(DisplayName="X"),
  EFX_Y UMETA(DisplayName="Y"),
};

UCLASS(Transient, BlueprintType, NotPlaceable)
class UFramesLayout : public UObject {
  GENERATED_UCLASS_BODY()
  ~UFramesLayout();

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  float BottomGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  FFramesRect BoundsGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  UFramesFrame *ChildGetByName(const FString &name) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  UFramesFrame *ChildImplementationGetByName(const FString &name) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  TArray<UFramesFrame *> ChildrenGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  TArray<UFramesFrame *> ChildrenImplementationGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Debug")
  void DebugLayoutDump() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Debug")
  FString DebugNameGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  UFramesEnvironment *EnvironmentGet() const;

  // Event* (needs event infrastructure)

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  float HeightGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Input")
  EFramesInputMode InputModeGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Input")
  void InputModeSet(EFramesInputMode mode);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  float LeftGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  FString NameGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  UFramesLayout *ParentGet() const;

  // PinGet (needs PinAxis/PinPoint; low-priority)

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  float PointGetAxis(EFramesAxis Axis, float Position) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  FVector2D PointGetAnchor(EFramesAnchor Anchor) const;
  
  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  FVector2D PointGetCoord(float X, float Y) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  FVector2D PointGetVector(FVector2D Anchor) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Input")
  UFramesLayout *ProbeAsMouse(float X, float Y) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  float RightGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  float SizeGet(EFramesAxis axis) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  float TopGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  FString TypeGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  bool VisibleGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  void VisibleSet(bool Visibility) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  float WidthGet() const;

  Frames::Layout *FramesLayoutGet() const { return m_layout; }

  // Checks to see if a layout is valid; outputs an error and returns false if it's not
  bool ValidCheck() const;

private:
  friend class FramesManager;

  Frames::Layout *m_layout;
};
