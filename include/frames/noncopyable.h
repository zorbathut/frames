// Noncopyable inheritance

#ifndef FRAMES_NONCOPYABLE
#define FRAMES_NONCOPYABLE

#include "frames/config.h"

#ifdef FRAMES_BOOST_ENABLED
#include <boost/noncopyable.hpp>
#endif

namespace Frames {
  #ifdef FRAMES_BOOST_ENABLED
    typedef boost::noncopyable Noncopyable
  #else
    #error this isn't actually tested yet!
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
