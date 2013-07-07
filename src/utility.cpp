
#include "frame/utility.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <cstdio>
#include <cstdarg>

// for the undefined types
BOOST_STATIC_ASSERT(sizeof(float) == sizeof(int));

namespace Frame {
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
        done = std::vsnprintf(&(buf[0]), buf.size() - 1,  bort, args);
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

