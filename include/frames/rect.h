// Rect class

#ifndef FRAMES_RECT
#define FRAMES_RECT

#include "frames/point.h"

namespace Frames {
  struct Rect {
    Rect() { }
    Rect(Point s, Point e) : s(s), e(e) { }
    Rect(float sx, float sy, float ex, float ey) : s(sx, sy), e(ex, ey) { }

    Point s;
    Point e;
  };
}

#endif
