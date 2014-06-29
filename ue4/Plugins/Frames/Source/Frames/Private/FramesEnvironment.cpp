
#include "FramesPCH.h"

#include "FramesEnvironment.h"

#include "AllowWindowsPlatformTypes.h"
#include <frames/environment.h>
#include <frames/configuration.h>
#include <frames/renderer_null.h>
#include "HideWindowsPlatformTypes.h"

UFramesEnvironment::UFramesEnvironment(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
  Frames::Configuration::Local conf;
  conf.RendererSet(Frames::Configuration::RendererNull());
  Frames::EnvironmentPtr env = Frames::Environment::Create(conf);

  //env->ResizeRoot

  //OutputDebugString(Frames::detail::format())
}

void UFramesEnvironment::Render(AHUD *hud)
{
  OutputDebugStringA("YARG");
}
