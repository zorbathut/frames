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
#include <frames/detail_format.h>

#include "lib.h"

void InputEventHook(VerbLog *log, Frames::Frame *frame) {
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

  log->Attach(frame, Frames::Layout::Event::KeyDown);
  log->Attach(frame, Frames::Layout::Event::KeyRepeat);
  log->Attach(frame, Frames::Layout::Event::KeyUp);
  log->Attach(frame, Frames::Layout::Event::KeyText);
}

TEST(Input, Ordering) {
  TestEnvironment env;

  // Create a series of frames, then test movement events
  // Three frames overlapping, a -> b -> c, though we generate them out of order to test layering
  Frames::Frame *a = Frames::Frame::Create(env->RootGet(), "a");
  Frames::Frame *c = Frames::Frame::Create(env->RootGet(), "c");
  Frames::Frame *b = Frames::Frame::Create(env->RootGet(), "b");

  a->LayerSet(0);
  b->LayerSet(1);
  c->LayerSet(2);

  a->WidthSet(400);
  a->HeightSet(300);
  b->WidthSet(400);
  b->HeightSet(300);
  c->WidthSet(400);
  c->HeightSet(300);

  a->BackgroundSet(Frames::Color(1, 0, 0, 0.8f));
  b->BackgroundSet(Frames::Color(0, 1, 0, 0.8f));
  c->BackgroundSet(Frames::Color(0, 0, 1, 0.8f));

  a->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 100, 100);
  b->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 200, 200);
  c->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 300, 300);
  
  // Just to make sure this hasn't changed, or we'll be confused
  TestSnapshot(env);

  // Make sure to reset to base state
  env->Input_MouseMove(0, 0);

  // very thorough testing
  for (int i = 0; i < (1 << 3); ++i) {
    a->InputModeSet((i & (1 << 0)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);
    b->InputModeSet((i & (1 << 1)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);
    c->InputModeSet((i & (1 << 2)) ? Frames::Layout::IM_ALL : Frames::Layout::IM_NONE);

    TestCompare compare("event");
    VerbLog log(&compare);

    InputEventHook(&log, a);
    InputEventHook(&log, b);
    InputEventHook(&log, c);

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
  Frames::Frame *a = Frames::Frame::Create(env->RootGet(), "a");
  Frames::Frame *b = Frames::Frame::Create(env->RootGet(), "b");

  a->WidthSet(400);
  a->HeightSet(300);
  b->WidthSet(400);
  b->HeightSet(300);

  a->BackgroundSet(Frames::Color(1, 0, 0, 0.8f));
  b->BackgroundSet(Frames::Color(0, 1, 0, 0.8f));

  a->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 100, 100);
  b->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 200, 200);

  a->InputModeSet(Frames::Layout::IM_ALL);
  b->InputModeSet(Frames::Layout::IM_ALL);

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
      TestCompare compare(Frames::detail::Format("frame%d_button%d", i, button));
      VerbLog log(&compare);

      InputEventHook(&log, a);
      InputEventHook(&log, b);

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

      TestCompare compare(Frames::detail::Format("frame%d_misc", i));
      VerbLog log(&compare);

      InputEventHook(&log, a);
      InputEventHook(&log, b);

      // Test mousewheel
      env->Input_MouseWheel(1);
    }

    env->Input_MouseClear();
  }

  // Test multiframe deals, for each button
  for (int button = 0; button < 4; ++button) {
    env->Input_MouseClear();

    TestCompare compare(Frames::detail::Format("multi_button%d", button));
    VerbLog log(&compare);

    InputEventHook(&log, a);
    InputEventHook(&log, b);

    env->Input_MouseMove(coord[0], coord[0]);
    env->Input_MouseDown(button);
    env->Input_MouseMove(coord[1], coord[1]);
    env->Input_MouseUp(button);
    env->Input_MouseDown(button);
    env->Input_MouseMove(coord[0], coord[0]);
    env->Input_MouseUp(button);
  }
}

TEST(Input, Meta) {
  TestEnvironment env;

  EXPECT_EQ(false, env->Input_MetaGet().shift);
  EXPECT_EQ(false, env->Input_MetaGet().alt);
  EXPECT_EQ(false, env->Input_MetaGet().ctrl);

  Frames::Input::Meta meta;
  meta.shift = true;
  meta.alt = true;
  meta.ctrl = true;

  env->Input_MetaSet(meta);

  EXPECT_EQ(true, env->Input_MetaGet().shift);
  EXPECT_EQ(true, env->Input_MetaGet().alt);
  EXPECT_EQ(true, env->Input_MetaGet().ctrl);

  // not exactly complicated
}

TEST(Input, Key) {
  TestEnvironment env;

  Frames::Frame *a = Frames::Frame::Create(env->RootGet(), "a");

  for (int i = 0; i < 3; ++i) {
    TestCompare compare;
    VerbLog log(&compare);

    InputEventHook(&log, a);

    env->FocusSet(i == 1 ? a : 0);

    env->Input_KeyDown(Frames::Input::H);
    env->Input_KeyDown(Frames::Input::I);

    env->Input_KeyRepeat(Frames::Input::I);
    
    env->Input_KeyUp(Frames::Input::I);
    env->Input_KeyUp(Frames::Input::H);

    env->Input_KeyText("Test1");
    env->Input_KeyText("Test2");
  }
}
