
#include <gtest/gtest.h>

#include <frames/frame.h>

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
    VerbLog log("basic");

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
    VerbLog log("bubble");

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
