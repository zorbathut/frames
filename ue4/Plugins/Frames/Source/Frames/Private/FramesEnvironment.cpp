
#include "FramesPCH.h"

#include "FramesEnvironment.h"

#include "AllowWindowsPlatformTypes.h"
#include <frames/configuration.h>
#include <frames/detail_format.h>
#include <frames/environment.h>
#include <frames/layout.h>
#include <frames/renderer_null.h>
#include "HideWindowsPlatformTypes.h"

UFramesEnvironment::UFramesEnvironment(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
  Frames::Configuration::Local conf;
  conf.RendererSet(Frames::Configuration::RendererNull());
  m_env = Frames::Environment::Create(conf);

  //env->ResizeRoot

  //OutputDebugString(Frames::detail::format())
}

void UFramesEnvironment::Render(AHUD *hud)
{
  m_env->ResizeRoot(hud->Canvas->SizeX, hud->Canvas->SizeY);
  m_env->Render();

  OutputDebugStringA(Frames::detail::Format("%f/%f", m_env->RootGet()->WidthGet(), m_env->RootGet()->HeightGet()).c_str());
}
