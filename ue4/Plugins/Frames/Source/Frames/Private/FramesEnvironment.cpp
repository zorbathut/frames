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
#include "FramesHUDHack.h"
#include "FramesRendererRHI.h"
#include "FramesFont.h"

#include "AllowWindowsPlatformTypes.h"
#include <frames/configuration.h>
#include <frames/detail_format.h>
#include <frames/environment.h>
#include <frames/frame.h>
#include <frames/layout.h>
#include <frames/renderer_null.h>
#include <frames/sprite.h>
#include <frames/stream.h>
#include <frames/text.h>
#include "HideWindowsPlatformTypes.h"

DEFINE_LOG_CATEGORY(FramesLog);

class FramesUE4Logger : Frames::Configuration::Logger
{
public:
  FramesUE4Logger() :
    m_onScreenErrors(true),
    m_assertErrors(false)
  {
  }

  virtual void LogError(const std::string &log)
  {
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

  virtual void LogDebug(const std::string &log)
  {
    UE_LOG(FramesLog, Log, TEXT("%s"), *FString(log.c_str()));
  }

private:
  bool m_onScreenErrors;
  bool m_assertErrors;
};

class UE4TextureFromId : public Frames::Configuration::TextureFromId {
  virtual Frames::TexturePtr Create(Frames::Environment *env, const std::string &id) {
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
};
typedef Frames::Ptr<UE4TextureFromId> UE4TextureFromIdPtr;

UE4TextureFromIdPtr GetUE4TextureFromId() {
  static UE4TextureFromIdPtr ue4tfip(new UE4TextureFromId());
  return ue4tfip;
}

class UE4StreamFromId : public Frames::Configuration::StreamFromId {
  virtual Frames::StreamPtr Create(Frames::Environment *env, const std::string &id) {
    UFramesFont *font = LoadObject<UFramesFont>(NULL, UTF8_TO_TCHAR(id.c_str()), NULL);
    if (!font) {
      return Frames::StreamPtr();
    }

    // UFramesFont has no streaming support, I think? it's still a bit unclear to me how this all works

    std::vector<unsigned char> data(font->Data.GetData(), font->Data.GetData() +  font->Data.Num());

    return Frames::StreamBuffer::Create(data);
  }
};
typedef Frames::Ptr<UE4StreamFromId> UE4StreamFromIdPtr;

UE4StreamFromIdPtr GetUE4StreamFromId() {
  static UE4StreamFromIdPtr ue4sfip(new UE4StreamFromId());
  return ue4sfip;
}

UFramesEnvironment::UFramesEnvironment(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
  Frames::Configuration::Local conf;
  conf.RendererSet(Frames::Configuration::RendererRHI());
  conf.LoggerSet(Frames::Configuration::LoggerPtr(new FramesUE4Logger()));
  conf.TextureFromIdSet(GetUE4TextureFromId());
  conf.StreamFromIdSet(GetUE4StreamFromId());
  m_env = Frames::Environment::Create(conf);
}

void UFramesEnvironment::Render(AHUD *hud)
{
  UCanvas *canvas = FramesHUDHack(hud);
  m_env->ResizeRoot(canvas->SizeX, canvas->SizeY);
  m_env->Render();
}
