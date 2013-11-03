// Win32-specific helper and example functions
// If these functions don't do quite what you want, you are encouraged to make copies of them and modify them.
// These are intended for the common case and as example code for more complicated things.

#ifndef FRAME_OSWIN32
#define FRAME_OSWIN32

#include "frames/input.h"

#include <windows.h>

namespace Frames {
  bool InputGatherWin32(InputEvent *event, HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam); // returns "true" if something useful has been gathered, false otherwise
}

#endif
