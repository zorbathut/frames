// The Color class

#ifndef FRAME_COLOR
#define FRAME_COLOR

namespace Frame {
  class Color {
  public:
    Color() : r(0), g(0), b(0), a(1) { };
    Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) { };
    union {
      struct { float r, g, b, a; };
      float c[4];
    };
  };

  inline bool operator==(const Color &lhs, const Color &rhs) {
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
  }

  inline bool operator!=(const Color &lhs, const Color &rhs) {
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
  }

  inline Color operator*(const Color &lhs, const Color &rhs) {
    return Color(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a);
  }
}

#endif
