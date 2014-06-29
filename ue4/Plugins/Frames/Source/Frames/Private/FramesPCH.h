
#pragma once

#include "CoreUObject.h"

// This is awful.
// We do it because there is otherwise almost no way to access the HUD's canvas; we'd have to ask people to inherit from HUD and provide a function.
// But, given that they *could* do this, we don't feel too bad about forcing access to that canvas.
// Still.
// Eww.
#define protected public
#include "Engine.h"
#undef protected
