
#pragma once

#include "AllowWindowsPlatformTypes.h"
#include <frames/environment.h>
#include <frames/configuration.h>
#include <frames/renderer_null.h>
#include "HideWindowsPlatformTypes.h"

#include "FramesEnvironment.generated.h"

/**
 * Frames main environment.
 */
UCLASS(Transient, Blueprintable, BlueprintType, NotPlaceable)
class UFramesEnvironment : public UObject
{
	GENERATED_UCLASS_BODY()

public:
  UFUNCTION(BlueprintCallable, Category="Render")
  void Render(AHUD *hud);

private:
  Frames::EnvironmentPtr m_env;
};
