
#include <gtest/gtest.h>

#include <frames/frame.h>
#include <frames/detail_format.h>

#include "lib.h"

void MouseEventHook(VerbLog *log, Frames::Frame *frame) {
  log->Attach(frame, Frames::Layout::Event::MouseOver);
  log->Attach(frame, Frames::Layout::Event::MouseMove);
  log->Attach(frame, Frames::Layout::Event::MouseMoveoutside);
  log->Attach(frame, Frames::Layout::Event::MouseOut);

  log->Attach(frame, Frames::Layout::Event::MouseLeftUp);
  log->Attach(frame, Frames::Layout::Event::MouseLeftUpoutside);
  log->Attach(frame, Frames::Layout::Event::MouseLeftDown);
  log->Attach(frame, Frames::Layout::Event::MouseLeftClick);

  log->Attach(frame, Frames::Layout::Event::MouseRightUp);
  log->Attach(frame, Frames::Layout::Event::MouseRightUpoutside);
  log->Attach(frame, Frames::Layout::Event::MouseRightDown);
  log->Attach(frame, Frames::Layout::Event::MouseRightClick);

  log->Attach(frame, Frames::Layout::Event::MouseMiddleUp);
  log->Attach(frame, Frames::Layout::Event::MouseMiddleUpoutside);
  log->Attach(frame, Frames::Layout::Event::MouseMiddleDown);
  log->Attach(frame, Frames::Layout::Event::MouseMiddleClick);

  log->Attach(frame, Frames::Layout::Event::MouseButtonUp);
  log->Attach(frame, Frames::Layout::Event::MouseButtonUpoutside);
  log->Attach(frame, Frames::Layout::Event::MouseButtonDown);
  log->Attach(frame, Frames::Layout::Event::MouseButtonClick);

  log->Attach(frame, Frames::Layout::Event::MouseWheel);
}

TEST(Input, Ordering) {
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

  a->SetBackground(Frames::Color(1, 0, 0, 0.8f));
  b->SetBackground(Frames::Color(0, 1, 0, 0.8f));
  c->SetBackground(Frames::Color(0, 0, 1, 0.8f));

  a->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT, 100, 100);
  b->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT, 200, 200);
  c->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT, 300, 300);
  
  // Just to make sure this hasn't changed, or we'll be confused
  TestSnapshot(env);

  // Make sure to reset to base state
  env->Input_MouseMove(0, 0);

  // very thorough testing
  for (int i = 0; i < (1 << 3); ++i) {
    a->SetInputMode((i & (1 << 0)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);
    b->SetInputMode((i & (1 << 1)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);
    c->SetInputMode((i & (1 << 2)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);

    VerbLog log;

    MouseEventHook(&log, a);
    MouseEventHook(&log, b);
    MouseEventHook(&log, c);

    env->Input_MouseMove(150, 150);
    env->Input_MouseDown(0);
    env->Input_MouseUp(0);

    env->Input_MouseMove(250, 250);
    env->Input_MouseDown(0);
    env->Input_MouseUp(0);

    env->Input_MouseMove(350, 350);
    env->Input_MouseDown(0);
    env->Input_MouseUp(0);

    env->Input_MouseMove(0, 0);
  }
}

TEST(Input, Mouse) {
  TestEnvironment env;

  // Two overlapping frames
  Frames::Frame *a = Frames::Frame::Create("a", env->GetRoot());
  Frames::Frame *b = Frames::Frame::Create("b", env->GetRoot());

  a->SetWidth(400);
  a->SetHeight(300);
  b->SetWidth(400);
  b->SetHeight(300);

  a->SetBackground(Frames::Color(1, 0, 0, 0.8f));
  b->SetBackground(Frames::Color(0, 1, 0, 0.8f));

  a->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT, 100, 100);
  b->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT, 200, 200);

  a->SetInputMode(Frames::Layout::IM_ALL);
  b->SetInputMode(Frames::Layout::IM_ALL);

  // Just to make sure this hasn't changed, or we'll be confused
  TestSnapshot(env);

  int coord[2] = {150, 250};

  // Reset to base state
  env->Input_MouseClear();

  // Test things that work on a single frame
  for (int i = 0; i < 2; ++i) {
    // Move mouse to the right place
    env->Input_MouseMove(coord[i], coord[i]);

    // For each button
    for (int button = 0; button < 4; ++button) {
      VerbLog log(Frames::detail::Format("frame%d_button%d", i, button));

      MouseEventHook(&log, a);
      MouseEventHook(&log, b);

      // Test basic click
      env->Input_MouseDown(button);
      env->Input_MouseUp(button);

      // Test drag within
      env->Input_MouseDown(button);
      env->Input_MouseMove(coord[i], coord[i] + 20);
      env->Input_MouseUp(button);
      env->Input_MouseMove(coord[i], coord[i]);

      // Test drag within and back
      env->Input_MouseDown(button);
      env->Input_MouseMove(coord[i], coord[i] + 20);
      env->Input_MouseMove(coord[i], coord[i]);
      env->Input_MouseUp(button);

      // Test drag outside
      env->Input_MouseDown(button);
      env->Input_MouseMove(coord[i], coord[i] + 400);
      env->Input_MouseMove(coord[i], coord[i] + 500);
      env->Input_MouseUp(button);
      env->Input_MouseMove(coord[i], coord[i]);

      // Test drag outside and back
      env->Input_MouseDown(button);
      env->Input_MouseMove(coord[i], coord[i] + 400);
      env->Input_MouseMove(coord[i], coord[i]);
      env->Input_MouseUp(button);
    }

    {
      env->Input_MouseMove(coord[i], coord[i]);

      VerbLog log(Frames::detail::Format("frame%d_misc", i));

      MouseEventHook(&log, a);
      MouseEventHook(&log, b);

      // Test mousewheel
      env->Input_MouseWheel(1);
    }

    env->Input_MouseClear();
  }

  // Test multiframe deals, for each button
  for (int button = 0; button < 4; ++button) {
    env->Input_MouseClear();

    VerbLog log(Frames::detail::Format("multi_button%d", button));

    MouseEventHook(&log, a);
    MouseEventHook(&log, b);

    env->Input_MouseMove(coord[0], coord[0]);
    env->Input_MouseDown(button);
    env->Input_MouseMove(coord[1], coord[1]);
    env->Input_MouseUp(button);
    env->Input_MouseDown(button);
    env->Input_MouseMove(coord[0], coord[0]);
    env->Input_MouseUp(button);
  }
}

/*TEST(Input, Meta) {

}

TEST(Input, Key) {

}
*/