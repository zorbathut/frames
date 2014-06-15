
#include "test.h"

#include "AllowWindowsPlatformTypes.h"
#include <frames/environment.h>
#include <frames/configuration.h>
#include <frames/renderer_dx11.h>
#include "HideWindowsPlatformTypes.h"

#include "FramesTest.h"

AFramesTest::AFramesTest(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
  Frames::Configuration::Local conf;
  conf.RendererSet(Frames::Configuration::RendererDX11(0));
  Frames::EnvironmentPtr env = Frames::Environment::Create(conf);
}


