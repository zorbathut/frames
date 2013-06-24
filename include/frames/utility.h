// Utility functions and items

#ifndef FRAMES_UTILITY
#define FRAMES_UTILITY

#include <string>

#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_lvalue_reference.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace Frames {
  enum Axis { X, Y }; // axes

  namespace Utility {
    template<typename T, typename U> T Reinterpret(U u) {
      union { T t; U u; } uni;
      uni.u = u;
      return uni.t;
    }

    const unsigned int Undefined_bitmask = 0xFFDEADFF;
    const float Undefined = Reinterpret<float>(Undefined_bitmask);
    inline bool IsUndefined(float x) {
      return Reinterpret<unsigned int>(x) == Undefined_bitmask;
    }

    const unsigned int Processing_bitmask = 0xFFCAFEFF;
    const float Processing = Reinterpret<float>(Processing_bitmask);
    inline bool IsProcessing(float x) {
      return Reinterpret<unsigned int>(x) == Processing_bitmask;
    }

    // not intended to be called by anything, just makes GCC shut up
    inline float RemoveUnusedVariableWarnings() {
      return Undefined + Processing;
    }

    std::string Format(const char *bort, ...) __attribute__((format(printf,1,2)));

    inline float Clamp(float current, float min, float max) {
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
