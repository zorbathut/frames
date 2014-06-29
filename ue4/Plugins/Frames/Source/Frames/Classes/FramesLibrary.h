
#pragma once

#include "FramesEnvironment.h"

#include "FramesLibrary.generated.h"

UCLASS(MinimalAPI)
class UFramesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

  /** Gets a vector parameter value from the material collection instance. Logs if ParameterName is invalid. */
	UFUNCTION(BlueprintCallable, Category="Frames", meta=(HidePin="WorldContextObject", DefaultToSelf="WorldContextObject"))
	static UFramesEnvironment *FramesEnvironmentCreate(UObject* WorldContextObject);
};
