// Point class

#ifndef FRAMES_POINT
#define FRAMES_POINT

#include <frames/utility.h>

namespace Frames {
  struct Point {
    Point() { };
    Point(float x, float y) : x(x), y(y) { };

    float x, y;
  };
  
  inline Point operator+(const Point &lhs, const Point &rhs) {
    return Point(lhs.x + rhs.x, lhs.y + rhs.y);
  }
  inline Point operator-(const Point &lhs, const Point &rhs) {
    return Point(lhs.x - rhs.x, lhs.y - rhs.y);
  }

  inline void operator+=(Point &lhs, const Point &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
  }
  inline void operator-=(Point &lhs, const Point &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
  }

  namespace Utility {
    inline Point Clamp(const Point &current, const Point &min, const Point &max) {
      return Point(Clamp(current.x, min.x, max.x), Clamp(current.y, min.y, max.y));
    }
  }
};

#endif
