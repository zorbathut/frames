// Compatibility shims for old versions of UE4 versions

#ifndef FRAMES_UE4_COMPAT
#define FRAMES_UE4_COMPAT

#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 6
#define FObjectInitializer FPostConstructInitializeProperties
#endif

#endif
