
#ifndef FRAMES_TEST_LIB
#define FRAMES_TEST_LIB

#include <SDL.h>

#include <frames/environment.h>
#include <frames/event.h>
#include <frames/layout.h>

#include <gtest/gtest.h>

namespace Frames {
  class Layout;
  template <typename Parameters> class Verb;
}

// Sets up a working test environment
class TestSDLEnvironment : Frames::detail::Noncopyable {
public:
  TestSDLEnvironment(int width, int height);
  ~TestSDLEnvironment();

  int WidthGet() const;
  int HeightGet() const;

  void Swap();
  void HandleEvents();

private:
  SDL_Window *m_win;
  SDL_GLContext m_glContext;

  int m_width;
  int m_height;
};

class TestLogger;

class TestEnvironment : Frames::detail::Noncopyable {
public:
  TestEnvironment(bool startSDL = true, int width = 1280, int height = 720);
  ~TestEnvironment();

  Frames::Environment *operator*() { return m_env.Get(); }
  Frames::Environment *operator->() { return m_env.Get(); }

  int WidthGet() const { return m_sdl->WidthGet(); }
  int HeightGet() const { return m_sdl->HeightGet(); }

  void Swap() { return m_sdl->Swap(); }
  void HandleEvents() { return m_sdl->HandleEvents(); }

  void AllowErrors();

private:
  TestSDLEnvironment *m_sdl; // mostly taken care of with constructor/destructor
  Frames::EnvironmentPtr m_env;

  Frames::Ptr<TestLogger> m_logger; // owned by m_env
};

class VerbLog : Frames::detail::Noncopyable {
public:
  VerbLog(const std::string &suffix = "");
  ~VerbLog();

  void Snapshot();  // auto-snapshot on exit
  template <typename Parameters> void Attach(Frames::Layout *layout, const Frames::Verb<Parameters> &verb) {
    EXPECT_TRUE(m_nameUniqueTest.count(layout->NameGet()) == 0 || m_nameUniqueTest[layout->NameGet()] == layout);

    typename Frames::Verb<Parameters>::TypeDelegate delegate = typename Frames::Verb<Parameters>::TypeDelegate(this, &VerbLog::RecordEvent);
    layout->EventAttach(verb, delegate);
    m_detachers.push_back(new Detacher<Parameters>(layout, verb, delegate));
  }

private:
  void RecordEvent(Frames::Handle *handle);
  void RecordEvent(Frames::Handle *handle, int p1);
  void RecordEvent(Frames::Handle *handle, const Frames::Vector &p1);
  void RecordEvent(Frames::Handle *handle, Frames::Input::Key p1);
  void RecordEvent(Frames::Handle *handle, const std::string &p1);

  void RecordResult(Frames::Handle *handle, const std::string &params);
  std::string m_records;

  std::string m_suffix;

  std::map<std::string, Frames::Layout *> m_nameUniqueTest;

  class DetacherBase : Frames::detail::Noncopyable {
  public:
    virtual ~DetacherBase() { }
  };

  template <typename Parameters> class Detacher : public DetacherBase {
  public:
    Detacher(Frames::Layout *layout, const Frames::Verb<Parameters> &verb, typename Frames::Verb<Parameters>::TypeDelegate delegate) : m_layout(layout), m_verb(verb), m_delegate(delegate) {};
    virtual ~Detacher() {
      m_layout->EventDetach(m_verb, m_delegate);
    }

  private:
    Frames::Layout *m_layout;
    const Frames::Verb<Parameters> &m_verb;
    typename Frames::Verb<Parameters>::TypeDelegate m_delegate;
  };

  std::vector<DetacherBase *> m_detachers;
};

void TestSnapshot(TestEnvironment &env, std::string fname = "");  // non-const-reference so we can just modify it
void HaltAndRender(TestEnvironment &env); // Intended for debugging with an attached graphics debugger

#endif
