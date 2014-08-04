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

#include "FramesConfigurationLocal.h"

#include "FramesStringutil.h"
#include "FramesConfigurationUE4.h"

#include "AllowWindowsPlatformTypes.h"
#include <frames/configuration.h>
#include "HideWindowsPlatformTypes.h"

UFramesConfigurationLocal::UFramesConfigurationLocal(const class FPostConstructInitializeProperties& PCIP)
  : Super(PCIP)
{
  // default values to link up with Unreal's environment
  m_config = Frames::Configuration::CreateUE4();
}

void UFramesConfigurationLocal::FontDefaultIdSet(const FString &Font) {
  m_config.FontDefaultIdSet(Frames::detail::UE4Convert(Font));
}

FString UFramesConfigurationLocal::FontDefaultIdGet() const {
  return Frames::detail::UE4Convert(m_config.FontDefaultIdGet());
}