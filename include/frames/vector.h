#ifndef FRAMES_VECTOR
#define FRAMES_VECTOR

#include <ostream>

namespace Frames {
  /// Represents a 2d location or direction.
  /** This is nothing more than a pair of floats tied together. As is usual with Vector classes, the actual meaning of this structure depends strongly on its context. */
  struct Vector {
    Vector() { };
    Vector(float x, float y) : x(x), y(y) { };

    float x, y;
  };
  std::ostream &operator<<(std::ostream &str, const Vector &rhs);
  
  inline Vector operator+(const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.x + rhs.x, lhs.y + rhs.y);
  }
  inline Vector operator-(const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.x - rhs.x, lhs.y - rhs.y);
  }

  inline void operator+=(Vector &lhs, const Vector &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
  }
  inline void operator-=(Vector &lhs, const Vector &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
  }

  namespace detail {
    // prototyped so we can call it without including utility.h
    float Clamp(float current, float min, float max);

    inline Vector Clamp(const Vector &current, const Vector &min, const Vector &max) {
      return Vector(Clamp(current.x, min.x, max.x), Clamp(current.y, min.y, max.y));
    }
  }
}

#endif
