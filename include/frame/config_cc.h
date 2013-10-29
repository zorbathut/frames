#ifndef FRAME_CONFIG_CC
#define FRAME_CONFIG_CC

#if defined(__GNUC__)
  #define FRAME_ATTRIBUTE_PRINTF(fmtidx, paramidx) __attribute__((format(printf,fmtidx,paramidx)))
#elif defined(_MSC_VER)
  #define FRAME_ATTRIBUTE_PRINTF(fmtidx, paramidx)  // not included at all
#else
  #warning Unknown compiler, falling back to no printf
  #define FRAME_ATTRIBUTE_PRINTF(fmtidx, paramidx)  // not included at all
#endif

#if defined(__GNUC__)
  #define FRAME_EXPECT(expression, expected) __builtin_expect((expression), (expected))
#elif defined(_MSC_VER)
  #define FRAME_EXPECT(expression, expected) (expression)
#else
  #warning Unknown compiler, falling back to no branch hinting
  #define FRAME_EXPECT(expression, expected) (expression)
#endif

#endif
