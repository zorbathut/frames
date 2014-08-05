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
