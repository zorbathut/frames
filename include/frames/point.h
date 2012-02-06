// Point class

#ifndef FRAMES_POINT
#define FRAMES_POINT

namespace Frames {
  struct Point {
    Point() { };
    Point(float x, float y) : x(x), y(y) { };

    float x, y;
  };
};

#endif
