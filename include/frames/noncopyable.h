// Noncopyable inheritance

#ifndef FRAME_NONCOPYABLE
#define FRAME_NONCOPYABLE

#include "frames/config.h"

#ifdef FRAME_BOOST_ENABLED
#include <boost/noncopyable.hpp>
#endif

namespace Frame {
  #ifdef FRAME_BOOST_ENABLED
    typedef boost::noncopyable Noncopyable;
  #else
    #error this isnt actually tested yet!
    class Noncopyable {
     protected:
        Noncopyable() {}
        ~Noncopyable() {}
     private:
        Noncopyable( const Noncopyable& );
        const Noncopyable& operator=( const Noncopyable& );
    };
  #endif
}

#endif
