// The Ptr adapter

#ifndef FRAMES_PTR
#define FRAMES_PTR

#include "frames/config.h"

#ifdef FRAMES_BOOST_ENABLED
#include <boost/smart_ptr/intrusive_ptr.hpp>
#endif

namespace Frames {
  #ifdef FRAMES_BOOST_ENABLED
    template <typename T> struct Ptr {
      typedef boost::intrusive_ptr<T> Type;
    };
  #else
    #error this isnt actually tested yet!
  #endif
  
  class Layout; typedef Ptr<Layout>::Type LayoutPtr;
  class Frame; typedef Ptr<Frame>::Type FramePtr;
}

#endif
