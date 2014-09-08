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

#include "FramesFrame.h"

#include "FramesConvert.h"

#include <frames/frame.h>

UFramesFrame::UFramesFrame(const FPostConstructInitializeProperties &PCIP)
  : Super(PCIP)
{

}

FLinearColor UFramesFrame::BackgroundGet() const {
  if (!ValidCheck()) return FLinearColor(0, 0, 0);

  return Frames::detail::UE4Convert(FramesFrameGet()->BackgroundGet());
}

void UFramesFrame::BackgroundSet(FLinearColor Color) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->BackgroundSet(Frames::detail::UE4Convert(Color));
}

void UFramesFrame::ConstraintClearAll() {
  if (!ValidCheck()) return;

  return FramesFrameGet()->ConstraintClearAll();
}

void UFramesFrame::HeightSet(float Size) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->HeightSet(Size);
}

bool UFramesFrame::ImplementationGet() const {
  if (!ValidCheck()) return false;

  return FramesFrameGet()->ImplementationGet();
}

void UFramesFrame::ImplementationSet(bool Implementation) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->ImplementationSet(Implementation);
}

float UFramesFrame::LayerGet() const {
  if (!ValidCheck()) return 0;

  return FramesFrameGet()->LayerGet();
}

void UFramesFrame::LayerSet(float Layer) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->LayerSet(Layer);
}

void UFramesFrame::NameSet(FString Name) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->NameSet(Frames::detail::UE4Convert(Name));
}

void UFramesFrame::Obliterate() {
  if (!ValidCheck()) return;

  return FramesFrameGet()->Obliterate();  // we intentionally don't disable now - we might need this UFramesFrame later if the obliterate isn't immediate
}

void UFramesFrame::ParentSet(UFramesLayout *Parent) {
  if (!Parent) {
    Frames::Configuration::Get().LoggerGet()->LogError("Attempted to use null Frames layout");
    return;
  }

  if (!ValidCheck() || !Parent->ValidCheck()) return;

  return FramesFrameGet()->ParentSet(Parent->FramesLayoutGet());
}

void UFramesFrame::PinClearAxis(EFramesAxis Axis, float Point) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->PinClear((Frames::Axis)Axis, Point);
}

void UFramesFrame::PinClearAnchor(EFramesAnchor Anchor) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->PinClear((Frames::Anchor)Anchor);
}

void UFramesFrame::PinClearCoord(float X, float Y) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->PinClear(X, Y);
}

void UFramesFrame::PinClearVector(FVector2D Position) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->PinClear(Frames::detail::UE4Convert(Position));
}

void UFramesFrame::PinClearAll(EFramesAxis Axis) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->PinClearAll((Frames::Axis)Axis);
}

void UFramesFrame::PinSetAnchorAnchor(EFramesAnchor Mine, UFramesLayout *Destination, EFramesAnchor Theirs, FVector2D Offset /*= FVector2D(0.f, 0.f)*/) {
  if (!Destination) {
    Frames::Configuration::Get().LoggerGet()->LogError("Attempted to use null Frames layout");
    return;
  }

  if (!ValidCheck() || !Destination->ValidCheck()) return;

  return FramesFrameGet()->PinSet((Frames::Anchor)Mine, Destination->FramesLayoutGet(), (Frames::Anchor)Theirs, Frames::detail::UE4Convert(Offset));
}

void UFramesFrame::PinSetAnchorProportional(EFramesAnchor Mine, UFramesLayout *Destination, FVector2D Theirs, FVector2D Offset /*= FVector2D(0.f, 0.f)*/) {
  if (!Destination) {
    Frames::Configuration::Get().LoggerGet()->LogError("Attempted to use null Frames layout");
    return;
  }

  if (!ValidCheck() || !Destination->ValidCheck()) return;

  return FramesFrameGet()->PinSet((Frames::Anchor)Mine, Destination->FramesLayoutGet(), Frames::detail::UE4Convert(Theirs), Frames::detail::UE4Convert(Offset));
}

void UFramesFrame::PinSetProportionalAnchor(FVector2D Mine, UFramesLayout *Destination, EFramesAnchor Theirs, FVector2D Offset /*= FVector2D(0.f, 0.f)*/) {
  if (!Destination) {
    Frames::Configuration::Get().LoggerGet()->LogError("Attempted to use null Frames layout");
    return;
  }

  if (!ValidCheck() || !Destination->ValidCheck()) return;

  return FramesFrameGet()->PinSet(Frames::detail::UE4Convert(Mine), Destination->FramesLayoutGet(), (Frames::Anchor)Theirs, Frames::detail::UE4Convert(Offset));
}

void UFramesFrame::PinSetProportionalProportional(FVector2D Mine, UFramesLayout *Destination, FVector2D Theirs, FVector2D Offset /*= FVector2D(0.f, 0.f)*/) {
  if (!Destination) {
    Frames::Configuration::Get().LoggerGet()->LogError("Attempted to use null Frames layout");
    return;
  }

  if (!ValidCheck() || !Destination->ValidCheck()) return;

  return FramesFrameGet()->PinSet(Frames::detail::UE4Convert(Mine), Destination->FramesLayoutGet(), Frames::detail::UE4Convert(Theirs), Frames::detail::UE4Convert(Offset));
}

void UFramesFrame::SizeClear(EFramesAxis Axis) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->SizeClear((Frames::Axis)Axis);
}

void UFramesFrame::SizeSet(EFramesAxis Axis, float Size) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->SizeSet((Frames::Axis)Axis, Size);
}

void UFramesFrame::WidthSet(float Size) {
  if (!ValidCheck()) return;

  return FramesFrameGet()->WidthSet(Size);
}

Frames::Frame *UFramesFrame::FramesFrameGet() const {
  return static_cast<Frames::Frame*>(FramesLayoutGet());
}