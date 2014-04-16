
#include "frames/detail.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <cstdio>
#include <cstdarg>

// for the undefined types
BOOST_STATIC_ASSERT(sizeof(float) == sizeof(int));

namespace Frames {
  namespace detail {
    const Point c_anchorLookup[ANCHOR_COUNT] = {
      Point(0.f, 0.f),  // TOPLEFT
      Point(0.f, 0.f),  // LEFTTOP
      Point(0.5f, 0.f),  // TOPCENTER
      Point(0.5f, 0.f),  // CENTERTOP
      Point(1.f, 0.f),  // TOPRIGHT
      Point(1.f, 0.f),  // RIGHTTOP
      Point(0.f, 0.5f),  // CENTERLEFT
      Point(0.f, 0.5f),  // LEFTCENTER
      Point(0.5f, 0.5f),  // CENTER
      Point(0.5f, 0.5f),  // CENTERCENTER
      Point(1.f, 0.5f),  // CENTERRIGHT
      Point(1.f, 0.5f),  // RIGHTCENTER
      Point(0.f, 1.f),  // BOTTOMLEFT
      Point(0.f, 1.f),  // LEFTBOTTOM
      Point(0.5f, 1.f),  // BOTTOMCENTER
      Point(0.5f, 1.f),  // CENTERBOTTOM
      Point(1.f, 1.f),  // BOTTOMRIGHT
      Point(1.f, 1.f),  // RIGHTBOTTOM

      // These constants are supposed to be Nil, but they're hardcoded to fix constant initialization order issues on some compilers. Can be fixed by putting them in a single file, in theory.
      Point(0.f, detail::Reinterpret<float>(0xFFF00DFF)),  // LEFT
      Point(0.5f, detail::Reinterpret<float>(0xFFF00DFF)),  // CENTERX
      Point(1.f, detail::Reinterpret<float>(0xFFF00DFF)),  // RIGHT
      Point(detail::Reinterpret<float>(0xFFF00DFF), 0.f),  // TOP
      Point(detail::Reinterpret<float>(0xFFF00DFF), 0.5f),  // CENTERY
      Point(detail::Reinterpret<float>(0xFFF00DFF), 1.f),  // BOTTOM
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

    BOOST_STATIC_ASSERT(sizeof(intfptr_t) >= sizeof(void (*)())); // technically it doesn't guarantee that all function pointers are the same. practically, they are, but we test this anyway when they're used
  }
}

