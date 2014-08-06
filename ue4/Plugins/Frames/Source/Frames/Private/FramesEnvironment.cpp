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

#include "FramesPCH.h"

#include "FramesEnvironment.h"

#include "FramesFont.h"
#include "FramesInput.h"
#include "FramesHUDHack.h"
#include "FramesRendererRHI.h"
#include "FramesStringutil.h"

#include <frames/configuration.h>
#include <frames/environment.h>

UFramesEnvironment::UFramesEnvironment(const class FPostConstructInitializeProperties& PCIP)
  : Super(PCIP)
{
}

void UFramesEnvironment::Initialize(const Frames::Configuration::Local &conf) {
  m_env = Frames::Environment::Create(conf);
}

void UFramesEnvironment::InputMouseMove(int32 X, int32 Y) {
  m_env->Input_MouseMove(X, Y);
}

void UFramesEnvironment::InputMouseDown(int32 Button) {
  m_env->Input_MouseDown(Button);
}

void UFramesEnvironment::InputMouseUp(int32 Button) {
  m_env->Input_MouseUp(Button);
}

void UFramesEnvironment::InputMouseWheel(int32 Delta) {
  m_env->Input_MouseWheel(Delta);
}

void UFramesEnvironment::InputMouseClear() {
  m_env->Input_MouseClear();
}

FVector2D UFramesEnvironment::InputMouseGet() const {
  Frames::Vector coord = m_env->Input_MouseGet();
  return FVector2D(coord.x, coord.y);
}

void UFramesEnvironment::InputMetaSet(FFramesInputMeta Meta) const {
  m_env->Input_MetaSet(Meta.ToFrames());
}

FFramesInputMeta UFramesEnvironment::InputMetaGet() const {
  return FFramesInputMeta(m_env->Input_MetaGet());
}

void UFramesEnvironment::Render(AHUD *hud) {
  UCanvas *canvas = FramesHUDHack(hud);
  m_env->ResizeRoot(canvas->SizeX, canvas->SizeY);
  m_env->Render();
}

void UFramesEnvironment::LogDebug(const FString &text) const {
  m_env->LogDebug(Frames::detail::UE4Convert(text));
}
 
void UFramesEnvironment::LogError(const FString &text) const {
  m_env->LogError(Frames::detail::UE4Convert(text));
}
