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

#include "lib.h"

#include <gtest/gtest.h>

#include <windows.h>

#undef main // dammit, sdl

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_AWAYMODE_REQUIRED);  // This isn't platform-agnostic, but it's good enough for now. Also we're just shotgunning all the "yes stay awake seriously" flags.
  if (argc > 1) {
    RendererIdSet(argv[1]);
    return RUN_ALL_TESTS();
  } else {
    int failure = 0;
    RendererIdSet("ogl3_2_compat");
    failure |= RUN_ALL_TESTS();
    if (failure) {
      printf("Failed in OGL module\n");
      return failure;
    }
    RendererIdSet("dx11_fl11");
    failure |= RUN_ALL_TESTS();
    if (failure) {
      printf("Failed in DX11 module\n");
      return failure;
    }
    return failure;
  }
}
