
#ifndef FRAMES_TEST_LIB_NULL
#define FRAMES_TEST_LIB_NULL

#include "lib.h"

class TestWindowNull : public TestWindow {
public:
  TestWindowNull(int width, int height);
  ~TestWindowNull();

  virtual void Swap();
  virtual void HandleEvents();

  virtual Frames::Configuration::RendererPtr RendererGet();

  virtual void ClearRenderTarget();
  virtual std::vector<unsigned char> Screenshot();
};

#endif
