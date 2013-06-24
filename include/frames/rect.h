// Rect class

#ifndef FRAME_RECT
#define FRAME_RECT

#include "frames/point.h"

namespace Frame {
  struct Rect {
    Rect() { }
    Rect(Point s, Point e) : s(s), e(e) { }
    Rect(float sx, float sy, float ex, float ey) : s(sx, sy), e(ex, ey) { }

    Point s;
    Point e;
  };
}

#endif
