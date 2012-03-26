// Utility functions and items

#ifndef FRAMES_UTILITY
#define FRAMES_UTILITY

#include <string>

namespace Frames {
  enum Axis { X, Y }; // axes

  namespace Utility {
    template<typename T, typename U> T Reinterpret(U u) {
      union { T t; U u; } uni;
      uni.u = u;
      return uni.t;
    }

    const unsigned int Undefined_bitmask = 0xFFDEADFF;
    const float Undefined = Reinterpret<float>(Undefined_bitmask);
    inline bool IsUndefined(float x) {
      return Reinterpret<unsigned int>(x) == Undefined_bitmask;
    }

    const unsigned int Processing_bitmask = 0xFFCAFEFF;
    const float Processing = Reinterpret<float>(Processing_bitmask);
    inline bool IsProcessing(float x) {
      return Reinterpret<unsigned int>(x) == Processing_bitmask;
    }

    std::string Format(const char *bort, ...) __attribute__((format(printf,1,2)));

    inline float Clamp(float current, float min, float max) {
      if (current < min) return min;
      if (current > max) return max;
      return current;
    }

    int ClampToPowerOf2(int input);
  }
}

#endif
