
#include "frames/detail.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <cstdio>
#include <cstdarg>

// for the undefined types
BOOST_STATIC_ASSERT(sizeof(float) == sizeof(int));

namespace Frames {
  namespace detail {
    const Vector c_anchorLookup[ANCHOR_COUNT] = {
      Vector(0.f, 0.f),  // TOPLEFT
      Vector(0.f, 0.f),  // LEFTTOP
      Vector(0.5f, 0.f),  // TOPCENTER
      Vector(0.5f, 0.f),  // CENTERTOP
      Vector(1.f, 0.f),  // TOPRIGHT
      Vector(1.f, 0.f),  // RIGHTTOP
      Vector(0.f, 0.5f),  // CENTERLEFT
      Vector(0.f, 0.5f),  // LEFTCENTER
      Vector(0.5f, 0.5f),  // CENTER
      Vector(0.5f, 0.5f),  // CENTERCENTER
      Vector(1.f, 0.5f),  // CENTERRIGHT
      Vector(1.f, 0.5f),  // RIGHTCENTER
      Vector(0.f, 1.f),  // BOTTOMLEFT
      Vector(0.f, 1.f),  // LEFTBOTTOM
      Vector(0.5f, 1.f),  // BOTTOMCENTER
      Vector(0.5f, 1.f),  // CENTERBOTTOM
      Vector(1.f, 1.f),  // BOTTOMRIGHT
      Vector(1.f, 1.f),  // RIGHTBOTTOM

      // These constants are supposed to be Nil, but they're hardcoded to fix constant initialization order issues on some compilers. Can be fixed by putting them in a single file, in theory.
      Vector(0.f, detail::Reinterpret<float>(0xFFF00DFF)),  // LEFT
      Vector(0.5f, detail::Reinterpret<float>(0xFFF00DFF)),  // CENTERX
      Vector(1.f, detail::Reinterpret<float>(0xFFF00DFF)),  // RIGHT
      Vector(detail::Reinterpret<float>(0xFFF00DFF), 0.f),  // TOP
      Vector(detail::Reinterpret<float>(0xFFF00DFF), 0.5f),  // CENTERY
      Vector(detail::Reinterpret<float>(0xFFF00DFF), 1.f),  // BOTTOM
    };

    int ClampToPowerOf2(int input) {
      input--;
      input = (input >> 1) | input;
      input = (input >> 2) | input;
      input = (input >> 4) | input;
      input = (input >> 8) | input;
      input = (input >> 16) | input;
      return input + 1;
    }
  }
}

