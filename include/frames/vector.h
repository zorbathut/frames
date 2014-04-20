#ifndef FRAMES_VECTOR
#define FRAMES_VECTOR

namespace Frames {
  struct Vector {
    Vector() { };
    Vector(float x, float y) : x(x), y(y) { };

    float x, y;
  };
  
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
    // prototyped
    float Clamp(float current, float min, float max);

    inline Vector Clamp(const Vector &current, const Vector &min, const Vector &max) {
      return Vector(Clamp(current.x, min.x, max.x), Clamp(current.y, min.y, max.y));
    }
  }
}

#endif
