
#include "FramesPCH.h"

#include "FramesLibrary.h"

UFramesLibrary::UFramesLibrary(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
  
}

/*static*/ UFramesEnvironment *UFramesLibrary::FramesEnvironmentCreate(UObject* WorldContextObject)
{
  return new UFramesEnvironment(FPostConstructInitializeProperties());
}
