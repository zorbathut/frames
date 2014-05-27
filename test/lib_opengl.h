
#ifndef FRAMES_TEST_LIB_OPENGL
#define FRAMES_TEST_LIB_OPENGL

#include "lib.h"

class TestWindowSDL : public TestWindow {
public:
  TestWindowSDL(int width, int height);
  ~TestWindowSDL();

  virtual void Swap();
  virtual void HandleEvents();

  virtual Frames::Configuration::RendererPtr RendererGet();

  virtual void ClearRenderTarget();
  virtual std::vector<unsigned char> Screenshot();

private:
  SDL_Window *m_win;
  SDL_GLContext m_glContext;
};

class TestWindowNullOGL : public TestWindow {
public:
  TestWindowNullOGL(int width, int height) : TestWindow(width, height) { }
  ~TestWindowNullOGL() { }

  virtual void Swap() { }
  virtual void HandleEvents() { }

  virtual Frames::Configuration::RendererPtr RendererGet();

  virtual void ClearRenderTarget() { }
  virtual std::vector<unsigned char> Screenshot() { return std::vector<unsigned char>(); }
};

#endif
