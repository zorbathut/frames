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

#include "FramesLayout.h"

#include <frames/frame.h>
#include <frames/rect.h>

#include "FramesConvert.h"
#include "FramesFrame.h"
#include "FramesManager.h"

// Can't just embed these into the enum thanks to limitations of the ue4 preprocessor
BOOST_STATIC_ASSERT(FIM_NONE == Frames::Layout::IM_NONE);
BOOST_STATIC_ASSERT(FIM_ALL == Frames::Layout::IM_ALL);

BOOST_STATIC_ASSERT(EFA_TOPLEFT == Frames::TOPLEFT);
BOOST_STATIC_ASSERT(EFA_TOPCENTER == Frames::TOPCENTER);
BOOST_STATIC_ASSERT(EFA_TOPRIGHT == Frames::TOPRIGHT);
BOOST_STATIC_ASSERT(EFA_CENTERLEFT == Frames::CENTERLEFT);
BOOST_STATIC_ASSERT(EFA_CENTER == Frames::CENTER);
BOOST_STATIC_ASSERT(EFA_CENTERRIGHT == Frames::CENTERRIGHT);
BOOST_STATIC_ASSERT(EFA_BOTTOMLEFT == Frames::BOTTOMLEFT);
BOOST_STATIC_ASSERT(EFA_BOTTOMCENTER == Frames::BOTTOMCENTER);
BOOST_STATIC_ASSERT(EFA_BOTTOMRIGHT == Frames::BOTTOMRIGHT);
BOOST_STATIC_ASSERT(EFA_LEFT == Frames::LEFT);
BOOST_STATIC_ASSERT(EFA_CENTERX == Frames::CENTERX);
BOOST_STATIC_ASSERT(EFA_RIGHT == Frames::RIGHT);
BOOST_STATIC_ASSERT(EFA_TOP == Frames::TOP);
BOOST_STATIC_ASSERT(EFA_CENTERY == Frames::CENTERY);
BOOST_STATIC_ASSERT(EFA_BOTTOM == Frames::BOTTOM);

BOOST_STATIC_ASSERT(EFX_X == Frames::X);
BOOST_STATIC_ASSERT(EFX_Y == Frames::Y);

UFramesLayout::UFramesLayout(const FPostConstructInitializeProperties &PCIP)
  : Super(PCIP)
{

}

UFramesLayout::~UFramesLayout() {
  FramesManager::Get().DestroyLayout(this);
}

float UFramesLayout::BottomGet() const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->BottomGet();
}

FFramesRect UFramesLayout::BoundsGet() const {
  if (!ValidCheck()) return FFramesRect(0, 0, 0, 0);

  return Frames::detail::UE4Convert(FramesLayoutGet()->BoundsGet());
}

UFramesFrame *UFramesLayout::ChildGetByName(const FString &name) const {
  if (!ValidCheck()) return 0;

  return Cast<UFramesFrame>(FramesManager::Get().Convert(FramesLayoutGet()->ChildGetByName(Frames::detail::UE4Convert(name))));
}

UFramesFrame *UFramesLayout::ChildImplementationGetByName(const FString &name) const {
  if (!ValidCheck()) return 0;

  return Cast<UFramesFrame>(FramesManager::Get().Convert(FramesLayoutGet()->ChildImplementationGetByName(Frames::detail::UE4Convert(name))));
}

TArray<UFramesFrame *> UFramesLayout::ChildrenGet() const {
  TArray<UFramesFrame *> rv;
  if (!ValidCheck()) return rv;

  const Frames::Layout::ChildrenList &list = FramesLayoutGet()->ChildrenGet();
  for (Frames::Layout::ChildrenList::const_iterator itr = list.begin(); itr != list.end(); ++itr) {
    rv.Push(Cast<UFramesFrame>(FramesManager::Get().Convert(*itr)));
  }
  return rv;
}

TArray<UFramesFrame *> UFramesLayout::ChildrenImplementationGet() const {
  TArray<UFramesFrame *> rv;
  if (!ValidCheck()) return rv;

  const Frames::Layout::ChildrenList &list = FramesLayoutGet()->ChildrenImplementationGet();
  for (Frames::Layout::ChildrenList::const_iterator itr = list.begin(); itr != list.end(); ++itr) {
    rv.Push(Cast<UFramesFrame>(FramesManager::Get().Convert(*itr)));
  }
  return rv;
}

void UFramesLayout::DebugLayoutDump() const {
  if (!ValidCheck()) return;

  return FramesLayoutGet()->DebugLayoutDump();
}

FString UFramesLayout::DebugNameGet() const {
  if (!ValidCheck()) return FString();

  return Frames::detail::UE4Convert(FramesLayoutGet()->DebugNameGet());
}

UFramesEnvironment *UFramesLayout::EnvironmentGet() const {
  if (!ValidCheck()) return 0;

  return FramesManager::Get().Convert(FramesLayoutGet()->EnvironmentGet());
}

float UFramesLayout::HeightGet() const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->HeightGet();
}

EFramesInputMode UFramesLayout::InputModeGet() const {
  if (!ValidCheck()) return (EFramesInputMode)0;

  return (EFramesInputMode)FramesLayoutGet()->InputModeGet();
}

void UFramesLayout::InputModeSet(EFramesInputMode mode) {
  if (!ValidCheck()) return;

  return FramesLayoutGet()->InputModeSet((Frames::Layout::InputMode)mode);
}

float UFramesLayout::LeftGet() const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->LeftGet();
}

FString UFramesLayout::NameGet() const {
  if (!ValidCheck()) return FString();

  return Frames::detail::UE4Convert(FramesLayoutGet()->NameGet());
}

UFramesLayout *UFramesLayout::ParentGet() const {
  if (!ValidCheck()) return 0;

  return FramesManager::Get().Convert(FramesLayoutGet()->ParentGet());
}

float UFramesLayout::PointGetAxis(EFramesAxis Axis, float Point) const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->PointGet((Frames::Axis)Axis, Point);
}

FVector2D UFramesLayout::PointGetAnchor(EFramesAnchor Anchor) const {
  if (!ValidCheck()) return FVector2D();

  return Frames::detail::UE4Convert(FramesLayoutGet()->PointGet((Frames::Anchor)Anchor));
}

FVector2D UFramesLayout::PointGetCoord(float X, float Y) const {
  if (!ValidCheck()) return FVector2D();

  return Frames::detail::UE4Convert(FramesLayoutGet()->PointGet(Frames::Vector(X, Y)));
}

FVector2D UFramesLayout::PointGetVector(FVector2D position) const {
  if (!ValidCheck()) return FVector2D();

  return Frames::detail::UE4Convert(FramesLayoutGet()->PointGet(Frames::detail::UE4Convert(position)));
}

UFramesLayout *UFramesLayout::ProbeAsMouse(float X, float Y) const {
  if (!ValidCheck()) return 0;

  return FramesManager::Get().Convert(FramesLayoutGet()->ProbeAsMouse(X, Y));
}

float UFramesLayout::RightGet() const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->RightGet();
}

float UFramesLayout::SizeGet(EFramesAxis Axis) const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->SizeGet((Frames::Axis)Axis);
}

float UFramesLayout::TopGet() const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->TopGet();
}

FString UFramesLayout::TypeGet() const {
  if (!ValidCheck()) return FString();

  return FramesLayoutGet()->TypeGet();
}

bool UFramesLayout::VisibleGet() const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->VisibleGet();
}

void UFramesLayout::VisibleSet(bool Visibility) const {
  if (!ValidCheck()) return;

  return FramesLayoutGet()->VisibleSet(Visibility);
}

float UFramesLayout::WidthGet() const {
  if (!ValidCheck()) return 0;

  return FramesLayoutGet()->WidthGet();
}

bool UFramesLayout::ValidCheck() const {
  if (!m_layout) {
    Frames::Configuration::Get().LoggerGet()->LogError("Attempted to use obliterated Frames layout");
  }

  return m_layout != 0;
}