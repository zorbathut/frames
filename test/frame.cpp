
#include <gtest/gtest.h>

#include <frames/frame.h>

#include "lib.h"

TEST(Layout, SetBackground) {
  TestEnvironment env;

  Frames::Frame *frame = Frames::Frame::Create("Test", env->RootGet());
  frame->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(1.f, 1.f, 1.f, 1.0f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.2f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(0.2f, 0.3f, 0.7f, 0.8f));
  TestSnapshot(env);
};

TEST(Layout, PinSet) {
  TestEnvironment env;

  Frames::Frame *red = Frames::Frame::Create("Test", env->RootGet());
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f)); // Partially transparent so we can see frame intersections

  red->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT);
  red->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::BOTTOMRIGHT);
  EXPECT_EQ(0, red->LeftGet());
  EXPECT_EQ(env.WidthGet(), red->RightGet());
  EXPECT_EQ(0, red->TopGet());
  EXPECT_EQ(env.HeightGet(), red->BottomGet());

  TestSnapshot(env);

  red->ConstraintClearAll();
  red->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT);
  red->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::CENTER);

  Frames::Frame *green = Frames::Frame::Create("Test", env->RootGet());
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  green->PinSet(Frames::CENTER, env->RootGet(), 0.75f, 0.75f);
  green->PinSet(Frames::TOPRIGHT, env->RootGet(), Frames::CENTERRIGHT);

  Frames::Frame *blue = Frames::Frame::Create("Test", env->RootGet());
  blue->SetBackground(Frames::Color(0.f, 0.f, 1.f, 0.5f));
  blue->PinSet(Frames::BOTTOMLEFT, env->RootGet(), Frames::CENTER);
  blue->PinSet(Frames::TOPRIGHT, env->RootGet(), Frames::TOPRIGHT);

  Frames::Frame *gray = Frames::Frame::Create("Test", env->RootGet());
  gray->SetBackground(Frames::Color(0.5f, 0.5f, 0.5f, 0.5f));
  gray->WidthSet((float)env.WidthGet() / 2);
  gray->HeightSet((float)env.HeightGet() / 2);
  gray->PinSet(Frames::CENTER, 0, Frames::Nil, Frames::Nil, (float)env.WidthGet() / 4, (float)env.HeightGet() / 4 * 3);

  TestSnapshot(env);

  red->ConstraintClearAll();
  green->ConstraintClearAll();
  blue->ConstraintClearAll();
  gray->ConstraintClearAll();

  red->PinSet(Frames::LEFT, env->RootGet(), Frames::LEFT);
  green->PinSet(Frames::LEFT, env->RootGet(), Frames::LEFT);
  blue->PinSet(Frames::LEFT, env->RootGet(), Frames::LEFT);
  gray->PinSet(Frames::LEFT, env->RootGet(), Frames::LEFT);

  red->PinSet(Frames::RIGHT, env->RootGet(), Frames::RIGHT);
  green->PinSet(Frames::RIGHT, env->RootGet(), Frames::RIGHT);
  blue->PinSet(Frames::RIGHT, env->RootGet(), Frames::RIGHT);
  gray->PinSet(Frames::RIGHT, env->RootGet(), Frames::RIGHT);

  red->PinSet(Frames::TOP, env->RootGet(), Frames::TOP);
  red->HeightSet(100.f);

  green->PinSet(Frames::TOP, red, Frames::BOTTOM);
  green->HeightSet(100.f);

  blue->PinSet(Frames::TOP, green, Frames::BOTTOM);
  blue->HeightSet(100.f);

  gray->PinSet(Frames::TOP, blue, Frames::BOTTOM);
  gray->HeightSet(100.f);
  
  TestSnapshot(env);
};

TEST(Layout, Layer) {
  TestEnvironment env;

  const int testFrameCount = 20;
  Frames::Frame *frames[testFrameCount];
  const float order[testFrameCount] = { 0, 6, 3.14159f, 3, 5, 9, 12, -5, 0, 0, 3.14159f, 2, 2, 2, 5000, 4999, 5001, 0, 3, 3.14159f };  // Numbers are not magic in any way, just trying to provide an interesting cross-section
  {
    Frames::Layout *anchor = env->RootGet();
    for (int i = 0; i < testFrameCount; ++i) {
      frames[i] = Frames::Frame::Create("Layer", env->RootGet());
      frames[i]->WidthSet(400.f);
      frames[i]->HeightSet(400.f);
      frames[i]->SetBackground(Frames::Color((float)i / testFrameCount, (float)i / testFrameCount, (float)i / testFrameCount, 0.2f));
      frames[i]->PinSet(Frames::TOPLEFT, anchor, Frames::TOPLEFT, 10.f, 10.f);
      frames[i]->LayerSet(order[i]);
      anchor = frames[i];
    }
  }

  TestSnapshot(env);

  // Re-order to do a new test
  for (int i = 0; i < testFrameCount; ++i) {
    frames[i]->LayerSet(order[(i + 3) % testFrameCount]);
  }

  TestSnapshot(env);

  // Test implementation
  for (int i = 0; i < testFrameCount; ++i) {
    frames[i]->ImplementationSet(i % 2 != 0);
  }

  TestSnapshot(env);

}

TEST(Layout, Error) {
  TestEnvironment env;
  TestEnvironment env2(false);

  env.AllowErrors();

  Frames::Frame *subject = Frames::Frame::Create("subject", env->RootGet());
  Frames::Frame *subjectalt = Frames::Frame::Create("subjectalt", env->RootGet());
  Frames::Frame *subj2 = Frames::Frame::Create("subject2", env2->RootGet());

  // Forged enums
  subject->PinClear(Frames::Anchor(-1));
  subject->PinClear(Frames::Anchor(Frames::ANCHOR_COUNT));

  subject->PinClearAll(Frames::Axis(-1));
  subject->PinClearAll(Frames::Axis(2));

  subject->SizeClear(Frames::Axis(-1));
  subject->SizeClear(Frames::Axis(2));

  subject->luaF_push(0);

  subject->InputModeSet(Frames::Layout::InputMode(-1));
  subject->InputModeSet(Frames::Layout::InputMode(2));

  subject->ParentSet(0);
  subject->ParentSet(subj2);

  subject->SizeSet(Frames::Axis(-1), 1);
  subject->SizeSet(Frames::Axis(2), 1);

  subject->PinSet(Frames::Anchor(-1), subjectalt, Frames::TOPLEFT);
  subject->PinSet(Frames::Anchor(Frames::ANCHOR_COUNT), subjectalt, Frames::TOPLEFT);
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::Anchor(-1));
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::Anchor(Frames::ANCHOR_COUNT));

  // Target errors
  subject->PinSet(Frames::TOPLEFT, subj2, Frames::TOPLEFT);
  subject->PinSet(Frames::TOPLEFT, subject, Frames::TOPLEFT);

  // Axis mismatches
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::TOP);
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::LEFT);
  subject->PinSet(Frames::TOP, subjectalt, Frames::TOPLEFT);
  subject->PinSet(Frames::LEFT, subjectalt, Frames::TOPLEFT);
  subject->PinSet(Frames::TOP, subjectalt, Frames::LEFT);

  // Offset mismatches
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::TOPLEFT, Frames::Nil, 0.0f);
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::TOPLEFT, 0.0f, Frames::Nil);
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::TOPLEFT, Frames::Nil, Frames::Nil);

  subject->PinSet(Frames::LEFT, subjectalt, Frames::LEFT, Frames::Nil, 0.0f);
  subject->PinSet(Frames::LEFT, subjectalt, Frames::LEFT, Frames::Nil, Frames::Nil);
  subject->PinSet(Frames::LEFT, subjectalt, Frames::LEFT, 0.0f, 0.0f);

  subject->PinSet(Frames::TOP, subjectalt, Frames::TOP, 0.0f, Frames::Nil);
  subject->PinSet(Frames::TOP, subjectalt, Frames::TOP, Frames::Nil, Frames::Nil);
  subject->PinSet(Frames::TOP, subjectalt, Frames::TOP, 0.0f, 0.0f);
}
