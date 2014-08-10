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

#include "FramesLayout.h"

#include "FramesFrame.generated.h"

UCLASS(Transient, BlueprintType, NotPlaceable)
class UFramesFrame : public UFramesLayout
{
  GENERATED_UCLASS_BODY()

  UFUNCTION(BlueprintCallable, Category="Frames|Frame")
  FLinearColor BackgroundGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Frame")
  void BackgroundSet(FLinearColor Background);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void ConstraintClearAll();

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void HeightSet(float Size);

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  bool ImplementationGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  void ImplementationSet(bool Implementation);

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  float LayerGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  void LayerSet(float Layer);

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  void NameSet(FString Name);

  UFUNCTION(BlueprintCallable, Category="Frames|Creation")
  void Obliterate();

  UFUNCTION(BlueprintCallable, Category="Frames|Hierarchy")
  void ParentSet(UFramesLayout *Parent);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void PinClearAxis(EFramesAxis Axis, float Point);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void PinClearAnchor(EFramesAnchor Anchor);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void PinClearCoord(float X, float Y);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void PinClearVector(FVector2D Position);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void PinClearAll(EFramesAxis Axis);

  // TODO: PinSet

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void SizeClear(EFramesAxis Axis);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void SizeSet(EFramesAxis Axis, float Size);

  UFUNCTION(BlueprintCallable, Category="Frames|Layout")
  void WidthSet(float Size);

  Frames::Frame *FrameGet() const;
};
