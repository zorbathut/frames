#ifndef FRAMES_COLOR
#define FRAMES_COLOR

namespace Frames {
  /// Stores a color value.
  /** Stores a standard RGBA color value with transparency. Each value is on a 0-to-1 scale, where 1 is fully lit or fully opaque. */
  class Color {
  public:
    Color() : r(0), g(0), b(0), a(1) { };
    Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) { };
    union {
      struct {
        /// Red color channel.
        float r;
        /// Green color channel.
        float g;
        /// Blue color channel.
        float b;
        /// Alpha color channel. 0 is transparent, 1 is opaque.
        float a;
      };
      /// Array representation, in RGBA order. Can be used interchangably with the other members.
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
