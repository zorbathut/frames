/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

#include <gtest/gtest.h>

#include <frames/frame.h>
#include <frames/event_definition.h>

#include "lib.h"

FRAMES_VERB_DEFINE(EventTestHelper, ());

void WheelEventHook(VerbLog *log, Frames::Layout *frame) {
  log->Attach(frame, Frames::Layout::Event::MouseWheel);
  log->Attach(frame, Frames::Layout::Event::MouseWheel.Dive);
  log->Attach(frame, Frames::Layout::Event::MouseWheel.Bubble);

  log->Attach(frame, Frames::Layout::Event::Move);
}

TEST(Event, Basic) {
  TestEnvironment env;

  Frames::Frame *branch = Frames::Frame::Create(env->RootGet(), "branch");
  Frames::Frame *leaf1 = Frames::Frame::Create(branch, "leaf1");
  Frames::Frame *leaf2 = Frames::Frame::Create(branch, "leaf2");

  {
    TestCompare compare("basic");
    VerbLog log(&compare);

    WheelEventHook(&log, env->RootGet());
    WheelEventHook(&log, branch);
    WheelEventHook(&log, leaf1);
    WheelEventHook(&log, leaf2);

    env->RootGet()->EventTrigger(Frames::Layout::Event::Move);
    branch->EventTrigger(Frames::Layout::Event::Move);
    leaf1->EventTrigger(Frames::Layout::Event::Move);
    leaf2->EventTrigger(Frames::Layout::Event::Move);
  }

  {
    TestCompare compare("bubble");
    VerbLog log(&compare);

    WheelEventHook(&log, env->RootGet());
    WheelEventHook(&log, branch);
    WheelEventHook(&log, leaf1);
    WheelEventHook(&log, leaf2);

    env->RootGet()->EventTrigger(Frames::Layout::Event::MouseWheel, 1);
    branch->EventTrigger(Frames::Layout::Event::MouseWheel, 1);
    leaf1->EventTrigger(Frames::Layout::Event::MouseWheel, 1);
    leaf2->EventTrigger(Frames::Layout::Event::MouseWheel, 1);
  }
}

TEST(Event, Ordering) {
  TestEnvironment env;

  Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Frame");

  {
    TestCompare compare;
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Frame");
    VerbLog llog(&compare, "botpri");
    VerbLog mlog(&compare, "midpri");
    VerbLog hlog(&compare, "highpri");

    llog.Attach(frame, EventTestHelper, -1);
    mlog.Attach(frame, EventTestHelper, 0);
    hlog.Attach(frame, EventTestHelper, 1);
    
    frame->EventTrigger(EventTestHelper);
  }

  {
    TestCompare compare;
    VerbLog llog(&compare, "botpri");
    VerbLog mlog(&compare, "midpri");
    VerbLog hlog(&compare, "highpri");

    hlog.Attach(frame, EventTestHelper, -98.5);
    mlog.Attach(frame, EventTestHelper, -99);
    llog.Attach(frame, EventTestHelper, -100);
    
    frame->EventTrigger(EventTestHelper);
  }

  {
    TestCompare compare;
    VerbLog llog(&compare, "botpri");
    VerbLog mlog(&compare, "midpri");
    VerbLog hlog(&compare, "highpri");

    mlog.Attach(frame, EventTestHelper, 50.002f);
    llog.Attach(frame, EventTestHelper, 50.001f);   
    hlog.Attach(frame, EventTestHelper, 50.003f);
    
    frame->EventTrigger(EventTestHelper);
  }
}

// Dynamic event creation mid-event
// Here's the plan:
// 0: print to A log
// 1: (n/a)
// 2: print to A log
// 3: Create new events printing to B log at 1 and 4
// 4: (n/a)
// 5: print to A log
// Should output A A B A
TEST(Event, Creation) {
  TestEnvironment env;
  TestCompare compare;

  struct Container : Frames::detail::Noncopyable {
    Container(TestEnvironment &tenv, TestCompare *compare) : compare(compare), alog(compare, "alpha"), blog(compare, "beta") {
      env = &tenv;
      f = Frames::Frame::Create((*env)->RootGet(), "f");
    }
    ~Container() {
      if (f) f->Obliterate();
    }

    void MakeEvents(Frames::Handle *) {
      compare->Append("Making");
      blog.Attach(f, EventTestHelper, 1);
      blog.Attach(f, EventTestHelper, 4);
    }

    Frames::Frame *f;
    TestEnvironment *env;

    TestCompare *compare;

    VerbLog alog;
    VerbLog blog;
  } container(env, &compare);

  container.alog.Attach(container.f, EventTestHelper, 0);
  container.alog.Attach(container.f, EventTestHelper, 2);
  container.alog.Attach(container.f, EventTestHelper, 5);

  container.f->EventAttach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::MakeEvents), 3);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventTestHelper);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventTestHelper);

  // Strip the event creator this time
  container.f->EventDetach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::MakeEvents), 3);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventTestHelper);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventTestHelper);
}

// Dynamic event destruction mid-event
// Here's the plan:
// 0: print to A log
// 1: print to dynlog
// 2: print to A log
// 3: destroy events at 1 and 5
// 4: destroy self
// 5: print to dynlog
// 6: print to A log
// Should output A dyn A A
TEST(Event, Destruction) {
  TestEnvironment env;
  TestCompare compare;

  struct Container : Frames::detail::Noncopyable {
    Container(TestEnvironment &tenv, TestCompare *compare) : compare(compare), alog(compare, "alpha") {
      env = &tenv;
      f = Frames::Frame::Create((*env)->RootGet(), "f");
    }
    ~Container() {
      if (f) f->Obliterate();
    }

    void DynLog(Frames::Handle *) {
      compare->Append("Event dynlog");
    }

    void RemoveEvents(Frames::Handle *) {
      compare->Append("Removing");
      f->EventDetach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(this, &Container::DynLog), 1);
      f->EventDetach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(this, &Container::DynLog), 5);
    }

    void DestroySelf(Frames::Handle *) {
      compare->Append("Selfremoving");
      f->EventDetach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(this, &Container::DestroySelf));
    }

    Frames::Frame *f;
    TestEnvironment *env;

    TestCompare *compare;

    VerbLog alog;
  } container(env, &compare);

  container.alog.Attach(container.f, EventTestHelper, 0);
  container.alog.Attach(container.f, EventTestHelper, 2);
  container.alog.Attach(container.f, EventTestHelper, 6);

  container.f->EventAttach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::DynLog), 1);
  container.f->EventAttach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::DynLog), 5);

  container.f->EventAttach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::RemoveEvents), 3);
  container.f->EventAttach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::DestroySelf), 4);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventTestHelper);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventTestHelper);

  // Strip the event remover, reattach the removed events
  container.f->EventAttach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::DynLog), 1);
  container.f->EventAttach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::DynLog), 5);
  container.f->EventDetach(EventTestHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::RemoveEvents), 3);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventTestHelper);
}
