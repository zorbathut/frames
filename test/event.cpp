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
  FRAMES_VERB_DEFINE(EventOrderingHelper, ());

  TestEnvironment env;

  Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Frame");

  {
    TestCompare compare;
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Frame");
    VerbLog llog(&compare, "botpri");
    VerbLog mlog(&compare, "midpri");
    VerbLog hlog(&compare, "highpri");

    llog.Attach(frame, EventOrderingHelper, -1);
    mlog.Attach(frame, EventOrderingHelper, 0);
    hlog.Attach(frame, EventOrderingHelper, 1);
    
    frame->EventTrigger(EventOrderingHelper);
  }

  {
    TestCompare compare;
    VerbLog llog(&compare, "botpri");
    VerbLog mlog(&compare, "midpri");
    VerbLog hlog(&compare, "highpri");

    hlog.Attach(frame, EventOrderingHelper, -98.5);
    mlog.Attach(frame, EventOrderingHelper, -99);
    llog.Attach(frame, EventOrderingHelper, -100);
    
    frame->EventTrigger(EventOrderingHelper);
  }

  {
    TestCompare compare;
    VerbLog llog(&compare, "botpri");
    VerbLog mlog(&compare, "midpri");
    VerbLog hlog(&compare, "highpri");

    mlog.Attach(frame, EventOrderingHelper, 50.002f);
    llog.Attach(frame, EventOrderingHelper, 50.001f);   
    hlog.Attach(frame, EventOrderingHelper, 50.003f);
    
    frame->EventTrigger(EventOrderingHelper);
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
FRAMES_VERB_DEFINE(EventCreationHelper, ());
TEST(Event, Creation) {
  TestEnvironment env;
  TestCompare compare;

  struct Container : Frames::detail::Noncopyable {
    Container(TestEnvironment &tenv, TestCompare *compare) : alog(compare, "alpha"), blog(compare, "beta") {
      env = &tenv;
      f = Frames::Frame::Create((*env)->RootGet(), "f");
    }
    ~Container() {
      if (f) f->Obliterate();
    }

    void MakeEvents(Frames::Handle *) {
      blog.Attach(f, EventCreationHelper, 1);
      blog.Attach(f, EventCreationHelper, 4);
    }

    Frames::Frame *f;
    TestEnvironment *env;

    VerbLog alog;
    VerbLog blog;
  } container(env, &compare);

  container.alog.Attach(container.f, EventCreationHelper, 0);
  container.alog.Attach(container.f, EventCreationHelper, 2);
  container.alog.Attach(container.f, EventCreationHelper, 5);

  container.f->EventAttach(EventCreationHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::MakeEvents), 3);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventCreationHelper);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventCreationHelper);

  // Strip the event creator this time
  container.f->EventDetach(EventCreationHelper, Frames::Delegate<void (Frames::Handle *)>(&container, &Container::MakeEvents), 3);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventCreationHelper);

  compare.Append("Event trigger");

  container.f->EventTrigger(EventCreationHelper);
}