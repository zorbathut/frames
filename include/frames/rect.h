// Rect class

#ifndef FRAMES_RECT
#define FRAMES_RECT

#include "frames/vector.h"

namespace Frames {
  struct Rect {
    Rect() { }
    Rect(Vector s, Vector e) : s(s), e(e) { }
    Rect(float sx, float sy, float ex, float ey) : s(sx, sy), e(ex, ey) { }

    Vector s;
    Vector e;
  };
}

#endif
