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

#include "FramesPCH.h"

#include "FramesText.h"

#include "FramesConvert.h"

BOOST_STATIC_ASSERT(EFIM_NONE == Frames::Text::INTERACTIVE_NONE);
BOOST_STATIC_ASSERT(EFIM_SELECT == Frames::Text::INTERACTIVE_SELECT);
BOOST_STATIC_ASSERT(EFIM_CURSOR == Frames::Text::INTERACTIVE_CURSOR);
BOOST_STATIC_ASSERT(EFIM_EDIT == Frames::Text::INTERACTIVE_EDIT);

UFramesText::UFramesText(const FPostConstructInitializeProperties &PCIP)
  : Super(PCIP)
{

}

FLinearColor UFramesText::ColorSelectionGet() const {
  if (!ValidCheck()) return FLinearColor(0, 0, 0);

  return Frames::detail::UE4Convert(FramesTextGet()->ColorSelectionGet());
}

void UFramesText::ColorSelectionSet(FLinearColor Selection) {
  if (!ValidCheck()) return;

  return FramesTextGet()->ColorSelectionSet(Frames::detail::UE4Convert(Selection));
}
  
FLinearColor UFramesText::ColorTextGet() const {
  if (!ValidCheck()) return FLinearColor(0, 0, 0);

  return Frames::detail::UE4Convert(FramesTextGet()->ColorTextGet());
}

void UFramesText::ColorTextSet(FLinearColor Text) {
  if (!ValidCheck()) return;

  return FramesTextGet()->ColorTextSet(Frames::detail::UE4Convert(Text));
}
  
FLinearColor UFramesText::ColorTextSelectedGet() const {
  if (!ValidCheck()) return FLinearColor(0, 0, 0);

  return Frames::detail::UE4Convert(FramesTextGet()->ColorTextSelectedGet());
}
  
void UFramesText::ColorTextSelectedSet(FLinearColor Selected) {
  if (!ValidCheck()) return;

  return FramesTextGet()->ColorTextSelectedSet(Frames::detail::UE4Convert(Selected));
}

int32 UFramesText::CursorGet() const {
  if (!ValidCheck()) return 0;

  return FramesTextGet()->CursorGet();
}

void UFramesText::CursorSet(int32 Position) {
  if (!ValidCheck()) return;

  return FramesTextGet()->CursorSet(Position);
}

UFramesFont *UFramesText::FontGet() const {
  if (!ValidCheck()) return nullptr;

  return LoadObject<UFramesFont>(NULL, *Frames::detail::UE4Convert(FramesTextGet()->FontGet()), NULL);
}

void UFramesText::FontSet(UFramesFont *Font) {
  if (!ValidCheck()) return;

  return FramesTextGet()->FontSet(Frames::detail::UE4Convert(Font->GetFullName()));
}
  
float UFramesText::FontSizeGet() const {
  if (!ValidCheck()) return 0.f;

  return FramesTextGet()->FontSizeGet();
}

void UFramesText::FontSizeSet(float Size) {
  if (!ValidCheck()) return;

  return FramesTextGet()->FontSizeSet(Size);
}

EFramesInteractivityMode UFramesText::InteractiveGet() const {
  if (!ValidCheck()) return EFIM_NONE;

  return (EFramesInteractivityMode)FramesTextGet()->InteractiveGet();
}

void UFramesText::InteractiveSet(EFramesInteractivityMode Interactivity) {
  if (!ValidCheck()) return;

  return FramesTextGet()->InteractiveSet((Frames::Text::InteractivityMode)Interactivity);
}
  
FVector2D UFramesText::ScrollGet() const {
  if (!ValidCheck()) return FVector2D(0, 0);

  return Frames::detail::UE4Convert(FramesTextGet()->ScrollGet());
}

void UFramesText::ScrollSet(const FVector2D &Scroll) {
  if (!ValidCheck()) return;

  return FramesTextGet()->ScrollSet(Frames::detail::UE4Convert(Scroll));
}

bool UFramesText::SelectionActiveGet() const {
  if (!ValidCheck()) return false;

  return FramesTextGet()->SelectionActiveGet();
}
  
int32 UFramesText::SelectionBeginGet() const {
  if (!ValidCheck()) return 0;

  return FramesTextGet()->SelectionBeginGet();
}

void UFramesText::SelectionClear() {
  if (!ValidCheck()) return;

  return FramesTextGet()->SelectionClear();
}
  
void UFramesText::SelectionSet(int32 Start, int32 End) {
  if (!ValidCheck()) return;

  return FramesTextGet()->SelectionSet(Start, End);
}
  
int32 UFramesText::SelectionEndGet() const {
  if (!ValidCheck()) return 0;

  return FramesTextGet()->SelectionEndGet();
}

FString UFramesText::TextGet() const {
  if (!ValidCheck()) return FString();

  return Frames::detail::UE4Convert(FramesTextGet()->TextGet());
}

void UFramesText::TextSet(const FString &Text) {
  if (!ValidCheck()) return;

  return FramesTextGet()->TextSet(Frames::detail::UE4Convert(Text));
}
  
bool UFramesText::WordwrapGet() const {
  if (!ValidCheck()) return false;

  return FramesTextGet()->WordwrapGet();
}

void UFramesText::WordwrapSet(bool Wordwrap) {
  if (!ValidCheck()) return;

  return FramesTextGet()->WordwrapSet(Wordwrap);
}
