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

#include "FramesCompat.h"
#include "FramesConfigurationUE4.h"
#include "FramesConvert.h"

#include <frames/configuration.h>
#include <frames/detail_format.h>

UFramesConfigurationLocal::UFramesConfigurationLocal(const FObjectInitializer &PCIP)
  : Super(PCIP)
{

}

void UFramesConfigurationLocal::Initialize(AActor *Context) {
  m_config = Frames::Configuration::CreateUE4(Context->GetWorld()->FeatureLevel);
}

void UFramesConfigurationLocal::FontDefaultIdSet(UFramesFont *Font) {
  m_config.FontDefaultIdSet(Frames::detail::UE4Convert(Font->GetPathName()));
}

UFramesFont *UFramesConfigurationLocal::FontDefaultIdGet() const {
  return LoadObject<UFramesFont>(NULL, *Frames::detail::UE4Convert(m_config.FontDefaultIdGet()), NULL);
}