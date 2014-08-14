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

#include "FramesFont.h"
#include "FramesFrame.h"

#include <frames/text.h>

#include "FramesText.generated.h"

UENUM()
enum EFramesInteractivityMode {
  EFIM_NONE UMETA(DisplayName="None"),
  EFIM_SELECT UMETA(DisplayName="Select"),
  EFIM_CURSOR UMETA(DisplayName="Cursor"),
  EFIM_EDIT UMETA(DisplayName="Edit"),
};

UCLASS(Transient, BlueprintType, NotPlaceable)
class UFramesText : public UFramesFrame {
  GENERATED_UCLASS_BODY()
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  FLinearColor ColorSelectionGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void ColorSelectionSet(FLinearColor Selection);
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  FLinearColor ColorTextGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void ColorTextSet(FLinearColor Text);
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  FLinearColor ColorTextSelectedGet() const;
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void ColorTextSelectedSet(FLinearColor Selected);

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  int32 CursorGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void CursorSet(int32 Position);

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  UFramesFont *FontGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void FontSet(UFramesFont *Font);
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  float FontSizeGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void FontSizeSet(float Size);

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  EFramesInteractivityMode InteractiveGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void InteractiveSet(EFramesInteractivityMode Interactivity);
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  FVector2D ScrollGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void ScrollSet(const FVector2D &Scroll);

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  bool SelectionActiveGet() const;
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  int32 SelectionBeginGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void SelectionClear();
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void SelectionSet(int32 Start, int32 End);
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  int32 SelectionEndGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  FString TextGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void TextSet(const FString &Text);
  
  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  bool WordwrapGet() const;

  UFUNCTION(BlueprintCallable, Category="Frames|Text")
  void WordwrapSet(bool Wordwrap);

  Frames::Text *FramesTextGet() const { return static_cast<Frames::Text*>(FramesLayoutGet()); }
};
