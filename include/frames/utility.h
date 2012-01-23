// Utility functions and items

#ifndef FRAMES_UTILITY
#define FRAMES_UTILITY

#include <boost/math/special_functions/fpclassify.hpp>

#include <string>

namespace Frames {
  namespace Utility {
    const float Undefined = 0.f/0.f;
    inline bool IsUndefined(float x) {
      return boost::math::isnan(x);
    }

    std::string Format(const char *bort, ...) __attribute__((format(printf,1,2)));
  }
}

#endif
