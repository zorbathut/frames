/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

// Win32-specific helper and example functions
// If these functions don't do quite what you want, you are encouraged to make copies of them and modify them.
// These are intended for the common case and as example code for more complicated things.

#ifndef FRAMES_OSWIN32
#define FRAMES_OSWIN32

#include "frames/input.h"

#include <windows.h>

namespace Frames {
  bool InputGatherWin32(Input::Sequence *event, HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam); // returns "true" if something useful has been gathered, false otherwise
}

#endif
