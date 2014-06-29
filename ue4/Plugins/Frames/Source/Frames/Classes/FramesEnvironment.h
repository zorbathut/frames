
#pragma once

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
};
