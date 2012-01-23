// Utility functions and items

#ifndef FRAMES_UTILITY
#define FRAMES_UTILITY

#include <boost/math/special_functions/fpclassify.hpp>

namespace Frames {
  namespace Utility {
    const float Undefined = 0.f/0.f;
    static bool IsUndefined(float x) {
      return boost::math::isnan(x);
    }
  }
}

#endif
