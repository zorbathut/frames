
#include "lib.h"

#include <gtest/gtest.h>

#include <windows.h>

#undef main // dammit, sdl

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);  // This isn't platform-agnostic, but it's good enough for now
  if (argc > 1) {
    RendererIdSet(argv[1]);
  }
  return RUN_ALL_TESTS();
}
