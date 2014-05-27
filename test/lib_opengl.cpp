
#include "lib_opengl.h"

#include <frames/renderer_opengl.h>

#define GLEW_STATIC
#include <GL/GLew.h>

TestWindowSDL::TestWindowSDL(int width, int height) : TestWindow(width, height), m_win(0), m_glContext(0) {
  EXPECT_EQ(0, SDL_Init(SDL_INIT_VIDEO));

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  m_win = SDL_CreateWindow("Frames test harness", 100, 100, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  EXPECT_TRUE(m_win != NULL);

  m_glContext = SDL_GL_CreateContext(m_win);
  EXPECT_TRUE(m_glContext != 0);
}

TestWindowSDL::~TestWindowSDL() {
  EXPECT_EQ(GL_NO_ERROR, glGetError()); // verify no GL issues

  if (m_glContext) {
    SDL_GL_DeleteContext(m_glContext);
  }

  if (m_win) {
    SDL_DestroyWindow(m_win);
  }

  SDL_Quit();
}

void TestWindowSDL::Swap() {
  SDL_GL_SwapWindow(m_win);
}

void TestWindowSDL::HandleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // We actually just ignore all events
  }
}

Frames::Configuration::RendererPtr TestWindowSDL::RendererGet() {
  return Frames::Configuration::RendererOpengl();
}

void TestWindowSDL::ClearRenderTarget() {
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
}

Frames::Configuration::RendererPtr TestWindowNullOGL::RendererGet() {
  return Frames::Configuration::RendererOpengl();
}
