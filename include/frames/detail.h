// Implementation details

#ifndef FRAMES_DETAIL
#define FRAMES_DETAIL

#include <string>

#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_lvalue_reference.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include "frames/config_cc.h"
#include "frames/const.h"
#include "frames/point.h"

namespace Frames {
  namespace Utility {
    extern const Point c_anchorLookup[ANCHOR_COUNT];

    template<typename T, typename U> T Reinterpret(U u) {
      union { T t; U u; } uni;
      uni.u = u;
      return uni.t;
    }
    
    inline bool IsNil(float x) {
      return Utility::Reinterpret<unsigned int>(x) == Utility::Reinterpret<unsigned int>(Nil);
    }

    const float Undefined = Reinterpret<float>(0xFFDEADFF);
    inline bool IsUndefined(float x) {
      return Reinterpret<unsigned int>(x) == Utility::Reinterpret<unsigned int>(Undefined);
    }

    const float Processing = Reinterpret<float>(0xFFCAFEFF);
    inline bool IsProcessing(float x) {
      return Reinterpret<unsigned int>(x) == Utility::Reinterpret<unsigned int>(Processing);
    }

    // not intended to be called by anything, just makes GCC shut up
    inline float RemoveUnusedVariableWarnings() {
      return Undefined + Processing + Nil;
    }

    std::string Format(const char *bort, ...) FRAMES_ATTRIBUTE_PRINTF(1, 2);

    inline float Clamp(float current, float min, float max) {
      if (current < min) return min;
      if (current > max) return max;
      return current;
    }
    inline int Clamp(int current, int min, int max) {
      if (current < min) return min;
      if (current > max) return max;
      return current;
    }

    int ClampToPowerOf2(int input);

    template<typename prefix, typename F> struct FunctionPrefix;
    template<typename prefix, typename R> struct FunctionPrefix<prefix, R ()> { typedef R T(prefix); };
    template<typename prefix, typename R, typename P1> struct FunctionPrefix<prefix, R (P1)> { typedef R T(prefix, P1); };
    template<typename prefix, typename R, typename P1, typename P2> struct FunctionPrefix<prefix, R (P1, P2)> { typedef R T(prefix, P1, P2); };
    template<typename prefix, typename R, typename P1, typename P2, typename P3> struct FunctionPrefix<prefix, R (P1, P2, P3)> { typedef R T(prefix, P1, P2, P3); };
    template<typename prefix, typename R, typename P1, typename P2, typename P3, typename P4> struct FunctionPrefix<prefix, R (P1, P2, P3, P4)> { typedef R T(prefix, P1, P2, P3, P4); };

    template<typename F> struct FunctionPtr;
    template<typename R> struct FunctionPtr<R ()> { typedef R (*T)(); };
    template<typename R, typename P1> struct FunctionPtr<R (P1)> { typedef R (*T)(P1); };
    template<typename R, typename P1, typename P2> struct FunctionPtr<R (P1, P2)> { typedef R (*T)(P1, P2); };
    template<typename R, typename P1, typename P2, typename P3> struct FunctionPtr<R (P1, P2, P3)> { typedef R (*T)(P1, P2, P3); };
    template<typename R, typename P1, typename P2, typename P3, typename P4> struct FunctionPtr<R (P1, P2, P3, P4)> { typedef R (*T)(P1, P2, P3, P4); };

    template<typename NR, typename F> struct FunctionRv;
    template<typename NR, typename R> struct FunctionRv<NR, R ()> { typedef NR T(); };
    template<typename NR, typename R, typename P1> struct FunctionRv<NR, R (P1)> { typedef NR T(P1); };
    template<typename NR, typename R, typename P1, typename P2> struct FunctionRv<NR, R (P1, P2)> { typedef NR T(P1, P2); };
    template<typename NR, typename R, typename P1, typename P2, typename P3> struct FunctionRv<NR, R (P1, P2, P3)> { typedef NR T(P1, P2, P3); };
    template<typename NR, typename R, typename P1, typename P2, typename P3, typename P4> struct FunctionRv<NR, R (P1, P2, P3, P4)> { typedef NR T(P1, P2, P3, P4); };
    
    template<typename IT> struct MakeConstRef { typedef typename boost::add_lvalue_reference<typename boost::add_const<typename boost::remove_reference<IT>::type>::type>::type T; };

    typedef intptr_t intfptr_t; // tested in utility.cpp
  };
}

#endif
