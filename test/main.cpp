
#include "lib.h"

#include <gtest/gtest.h>

#undef main // dammit, sdl

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  if (argc > 1) {
    RendererIdSet(argv[1]);
  }
  return RUN_ALL_TESTS();
}
