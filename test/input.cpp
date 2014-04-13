
#include <gtest/gtest.h>

#include <frames/frame.h>

#include "lib.h"

TEST(Input, Focus) {
  TestEnvironment env;

  // Create a series of frames, then test movement events
  // Three frames overlapping, a -> b -> c, though we generate them out of order to test layering
  Frames::Frame *a = Frames::Frame::Create("a", env->GetRoot());
  Frames::Frame *c = Frames::Frame::Create("c", env->GetRoot());
  Frames::Frame *b = Frames::Frame::Create("b", env->GetRoot());

  a->SetLayer(0);
  b->SetLayer(1);
  c->SetLayer(2);

  a->SetWidth(400);
  a->SetHeight(300);
  b->SetWidth(400);
  b->SetHeight(300);
  c->SetWidth(400);
  c->SetHeight(300);

  a->SetBackground(Frames::Color(1, 0, 0, 0.8));
  b->SetBackground(Frames::Color(0, 1, 0, 0.8));
  c->SetBackground(Frames::Color(0, 0, 1, 0.8));

  a->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT, 100, 100);
  b->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT, 200, 200);
  c->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT, 300, 300);
  
  // Just to make sure this hasn't changed, or we'll be confused
  TestSnapshot(env);

  // Make sure to reset to base state
  env->MouseMove(0, 0);

  // very thorough testing
  for (int i = 0; i < (1 << 3); ++i) {
    if (i == 0) {
      continue; // no actual input hooks, this is silly
    }

    a->SetInputMode((i & (1 << 0)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);
    b->SetInputMode((i & (1 << 1)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);
    c->SetInputMode((i & (1 << 2)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);

    VerbLog log;

    log.Attach(a, Frames::Layout::Event::MouseOver);
    log.Attach(a, Frames::Layout::Event::MouseOut);
    log.Attach(a, Frames::Layout::Event::MouseLeftDown);

    log.Attach(b, Frames::Layout::Event::MouseOver);
    log.Attach(b, Frames::Layout::Event::MouseOut);
    log.Attach(b, Frames::Layout::Event::MouseLeftDown);

    log.Attach(c, Frames::Layout::Event::MouseOver);
    log.Attach(c, Frames::Layout::Event::MouseOut);
    log.Attach(c, Frames::Layout::Event::MouseLeftDown);

    env->MouseMove(150, 150);
    env->MouseDown(0);
    env->MouseUp(0);

    env->MouseMove(250, 250);
    env->MouseDown(0);
    env->MouseUp(0);

    env->MouseMove(350, 350);
    env->MouseDown(0);
    env->MouseUp(0);

    env->MouseMove(0, 0);
  }
}
