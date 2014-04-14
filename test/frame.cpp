
#include <gtest/gtest.h>

#include <frames/frame.h>

#include "lib.h"

TEST(Layout, SetBackground) {
  TestEnvironment env;

  Frames::Frame *frame = Frames::Frame::Create("Test", env->GetRoot());
  frame->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(1.f, 1.f, 1.f, 1.0f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.2f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(0.2f, 0.3f, 0.7f, 0.8f));
  TestSnapshot(env);
};

TEST(Layout, SetPin) {
  TestEnvironment env;

  Frames::Frame *red = Frames::Frame::Create("Test", env->GetRoot());
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f)); // Partially transparent so we can see frame intersections

  red->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  red->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMRIGHT);
  EXPECT_EQ(red->GetLeft(), 0);
  EXPECT_EQ(red->GetRight(), env.GetWidth());
  EXPECT_EQ(red->GetTop(), 0);
  EXPECT_EQ(red->GetBottom(), env.GetHeight());

  TestSnapshot(env);

  red->ClearConstraintAll();
  red->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  red->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::CENTER);

  Frames::Frame *green = Frames::Frame::Create("Test", env->GetRoot());
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  green->SetPin(Frames::CENTER, env->GetRoot(), 0.75f, 0.75f);
  green->SetPin(Frames::TOPRIGHT, env->GetRoot(), Frames::CENTERRIGHT);

  Frames::Frame *blue = Frames::Frame::Create("Test", env->GetRoot());
  blue->SetBackground(Frames::Color(0.f, 0.f, 1.f, 0.5f));
  blue->SetPin(Frames::BOTTOMLEFT, env->GetRoot(), Frames::CENTER);
  blue->SetPin(Frames::TOPRIGHT, env->GetRoot(), Frames::TOPRIGHT);

  Frames::Frame *gray = Frames::Frame::Create("Test", env->GetRoot());
  gray->SetBackground(Frames::Color(0.5f, 0.5f, 0.5f, 0.5f));
  gray->SetWidth((float)env.GetWidth() / 2);
  gray->SetHeight((float)env.GetHeight() / 2);
  gray->SetPin(Frames::CENTER, 0, Frames::Nil, Frames::Nil, (float)env.GetWidth() / 4, (float)env.GetHeight() / 4 * 3);

  TestSnapshot(env);

  red->ClearConstraintAll();
  green->ClearConstraintAll();
  blue->ClearConstraintAll();
  gray->ClearConstraintAll();

  red->SetPin(Frames::LEFT, env->GetRoot(), Frames::LEFT);
  green->SetPin(Frames::LEFT, env->GetRoot(), Frames::LEFT);
  blue->SetPin(Frames::LEFT, env->GetRoot(), Frames::LEFT);
  gray->SetPin(Frames::LEFT, env->GetRoot(), Frames::LEFT);

  red->SetPin(Frames::RIGHT, env->GetRoot(), Frames::RIGHT);
  green->SetPin(Frames::RIGHT, env->GetRoot(), Frames::RIGHT);
  blue->SetPin(Frames::RIGHT, env->GetRoot(), Frames::RIGHT);
  gray->SetPin(Frames::RIGHT, env->GetRoot(), Frames::RIGHT);

  red->SetPin(Frames::TOP, env->GetRoot(), Frames::TOP);
  red->SetHeight(100.f);

  green->SetPin(Frames::TOP, red, Frames::BOTTOM);
  green->SetHeight(100.f);

  blue->SetPin(Frames::TOP, green, Frames::BOTTOM);
  blue->SetHeight(100.f);

  gray->SetPin(Frames::TOP, blue, Frames::BOTTOM);
  gray->SetHeight(100.f);
  
  TestSnapshot(env);
};

TEST(Layout, Layer) {
  TestEnvironment env;

  const int testFrameCount = 20;
  Frames::Frame *frames[testFrameCount];
  const float order[testFrameCount] = { 0, 6, 3.14159f, 3, 5, 9, 12, -5, 0, 0, 3.14159f, 2, 2, 2, 5000, 4999, 5001, 0, 3, 3.14159f };  // Numbers are not magic in any way, just trying to provide an interesting cross-section
  {
    Frames::Layout *anchor = env->GetRoot();
    for (int i = 0; i < testFrameCount; ++i) {
      frames[i] = Frames::Frame::Create("Layer", env->GetRoot());
      frames[i]->SetWidth(400.f);
      frames[i]->SetHeight(400.f);
      frames[i]->SetBackground(Frames::Color((float)i / testFrameCount, (float)i / testFrameCount, (float)i / testFrameCount, 0.2f));
      frames[i]->SetPin(Frames::TOPLEFT, anchor, Frames::TOPLEFT, 10.f, 10.f);
      frames[i]->SetLayer(order[i]);
      anchor = frames[i];
    }
  }

  TestSnapshot(env);

  // Re-order to do a new test
  for (int i = 0; i < testFrameCount; ++i) {
    frames[i]->SetLayer(order[(i + 3) % testFrameCount]);
  }

  TestSnapshot(env);

  // Test implementation
  for (int i = 0; i < testFrameCount; ++i) {
    frames[i]->SetImplementation(i % 2 != 0);
  }

  TestSnapshot(env);
}