
#include <SDL.h>
#include <frames/environment.h>

// Sets up a working test environment
class TestSDLEnvironment {
public:
  TestSDLEnvironment();
  ~TestSDLEnvironment();

  int GetWidth() const;
  int GetHeight() const;

private:
  SDL_Window *m_win;
  SDL_GLContext m_glContext;

  int m_width;
  int m_height;
};

class TestEnvironment {
public:
  TestEnvironment();
  ~TestEnvironment();

  Frames::Environment *operator*() { return m_env; }
  Frames::Environment *operator->() { return m_env; }

  int GetWidth() const { return m_sdl.GetWidth(); }
  int GetHeight() const { return m_sdl.GetHeight(); }

private:
  TestSDLEnvironment m_sdl; // mostly taken care of with constructor/destructor
  Frames::Environment *m_env;
};

void TestSnapshot(TestEnvironment &env);