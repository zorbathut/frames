
#ifndef FRAMES_TEST_LIB
#define FRAMES_TEST_LIB

#include <SDL.h>

#include <frames/environment.h>
#include <frames/event.h>

namespace Frames {
  class Layout;
  template <typename Parameters> class Verb;
}

// Sets up a working test environment
class TestSDLEnvironment {
public:
  TestSDLEnvironment();
  ~TestSDLEnvironment();

  int GetWidth() const;
  int GetHeight() const;

  void Swap();
  void HandleEvents();

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

  void Swap() { return m_sdl.Swap(); }
  void HandleEvents() { return m_sdl.HandleEvents(); }

private:
  TestSDLEnvironment m_sdl; // mostly taken care of with constructor/destructor
  Frames::Environment *m_env;
};

class VerbLog {
public:
  VerbLog();
  ~VerbLog();

  void Snapshot();  // auto-snapshot on exit
  template <typename Parameters> void Attach(Frames::Layout *layout, const Frames::Verb<Parameters> &verb) {
    EXPECT_TRUE(m_nameUniqueTest.count(layout->GetName()) == 0 || m_nameUniqueTest[layout->GetName()] == layout);

    layout->EventAttach(verb, Frames::Delegate<typename Frames::detail::FunctionPrefix<Frames::Handle*, Parameters>::T>(this, &VerbLog::RecordEvent));
  }

private:
  void RecordEvent(Frames::Handle *);

  void RecordResult(const std::string &str);
  std::string m_records;

  std::map<std::string, Frames::Layout *> m_nameUniqueTest;
};

void TestSnapshot(TestEnvironment &env);
void HaltAndRender(TestEnvironment &env); // Intended for debugging with an attached graphics debugger

#endif
