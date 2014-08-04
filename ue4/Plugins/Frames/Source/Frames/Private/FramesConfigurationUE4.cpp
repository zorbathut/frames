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

#include "FramesConfigurationUE4.h"

#include "FramesFont.h"
#include "FramesRendererRHI.h"
#include "FramesUtility.h"

#include "AllowWindowsPlatformTypes.h"
#include <frames/configuration.h>
#include <frames/stream.h>
#include "HideWindowsPlatformTypes.h"

Frames::Configuration::Local Frames::Configuration::CreateUE4() {
  Frames::Configuration::Local conf;
  conf.RendererSet(Frames::Configuration::RendererRHI());
  conf.LoggerSet(Frames::Configuration::LoggerPtr(new Frames::Configuration::LoggerUE4(true, false)));
  conf.TextureFromIdSet(Frames::Configuration::TextureFromIdPtr(new Frames::Configuration::TextureFromIdUE4()));
  conf.StreamFromIdSet(Frames::Configuration::StreamFromIdPtr(new Frames::Configuration::StreamFromIdUE4()));
  return conf;
}

Frames::Configuration::LoggerUE4::LoggerUE4(bool errorScreen, bool errorAssert) :
  m_onScreenErrors(errorScreen),
  m_assertErrors(errorAssert)
{
}

void Frames::Configuration::LoggerUE4::LogError(const std::string &log) {
  UE_LOG(FramesLog, Error, TEXT("%s"), *FString(log.c_str()));  // This could probably be a *lot* more efficient

  if (m_onScreenErrors)
  {
    GEngine->AddOnScreenDebugMessage((int32)-1, 2.0f, FLinearColor(0.f,1.f,1.f).ToFColor(true), log.c_str());  // It's possible something like this should be introduced for debug mode; worry about this!
  }
    
  if (m_assertErrors)
  {
    checkf(false && "Frames internal error", TEXT("%s"), *FString(log.c_str()));
  }
}

void Frames::Configuration::LoggerUE4::LogDebug(const std::string &log) {
  UE_LOG(FramesLog, Log, TEXT("%s"), *FString(log.c_str()));
}

Frames::TexturePtr Frames::Configuration::TextureFromIdUE4::Create(Frames::Environment *env, const std::string &id) {
  UTexture2D *tex = LoadObject<UTexture2D>(NULL, UTF8_TO_TCHAR(id.c_str()), NULL);
  if (!tex) {
    return Frames::TexturePtr();
  }

  // Frames does not yet have good support for asynchronous loading, so we just force immediate loading
  tex->WaitForStreaming();

  // We really want an FTexture2DRHIRef
  FTexture2DRHIRef ref = static_cast<FRHITexture2D *>(tex->TextureReference.TextureReferenceRHI->GetReferencedTexture());

  // Wrap this up in the appropriate structures
  Frames::detail::UE4TextureContextualPtr contextual(new Frames::detail::UE4TextureContextual);
  contextual->m_tex = ref;

  // LAUNCH
  return Frames::Texture::CreateContextual(env, ref->GetSizeX(), ref->GetSizeY(), Frames::Texture::FORMAT_RGBA_8, contextual);
}

Frames::StreamPtr Frames::Configuration::StreamFromIdUE4::Create(Frames::Environment *env, const std::string &id) {
  UFramesFont *font = LoadObject<UFramesFont>(NULL, UTF8_TO_TCHAR(id.c_str()), NULL);
  if (!font) {
    return Frames::StreamPtr();
  }

  // UFramesFont has no streaming support, I think? it's still a bit unclear to me how this all works

  std::vector<unsigned char> data(font->Data.GetData(), font->Data.GetData() +  font->Data.Num());

  return Frames::StreamBuffer::Create(data);
}
