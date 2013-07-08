// OS-specific functionality and helper functions

#ifndef FRAME_OS
#define FRAME_OS

#include "frame/input.h"

#include <windows.h>

namespace Frame {
  bool InputGatherWin32(InputEvent *event, HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam); // returns "true" if something useful has been gathered, false otherwise
}

#endif
