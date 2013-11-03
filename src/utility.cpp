
#include "frames/utility.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <cstdio>
#include <cstdarg>

// for the undefined types
BOOST_STATIC_ASSERT(sizeof(float) == sizeof(int));

namespace Frames {
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
    Point(0.f, Nil),  // LEFT
    Point(0.5f, Nil),  // CENTERX
    Point(1.f, Nil),  // RIGHT
    Point(Nil, 0.f),  // TOP
    Point(Nil, 0.5f),  // CENTERY
    Point(Nil, 1.f),  // BOTTOM
  };

  namespace Utility {
    std::string Format(const char *bort, ...) {
      // NOTE: this function isn't particularly efficient, but is used only for debug output anyway
      std::vector<char> buf(1000);
      std::va_list args;

      int done = 0;
      bool noresize = false;
      do {
        if(done && !noresize)
          buf.resize(buf.size() * 2);
        va_start(args, bort);
        done = vsnprintf(&(buf[0]), buf.size() - 1,  bort, args);
        if(done >= (int)buf.size()) {
          buf.resize(done + 2);
          done = -1;
          noresize = true;
        }
        va_end(args);
      } while(done == (int)buf.size() - 1 || done == -1);

      std::string rv = std::string(buf.begin(), buf.begin() + done);

      return rv;
    }

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

