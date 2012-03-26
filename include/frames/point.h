// Point class

#ifndef FRAMES_POINT
#define FRAMES_POINT

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
};

#endif
