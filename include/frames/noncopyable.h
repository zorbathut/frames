// Noncopyable inheritance

#ifndef FRAMES_NONCOPYABLE
#define FRAMES_NONCOPYABLE

#include "frames/config.h"

#ifdef FRAMES_BOOST_ENABLED
#include <boost/noncopyable.hpp>
#endif

namespace Frames {
  namespace detail {
    typedef boost::noncopyable Noncopyable;
  }
}

#endif
