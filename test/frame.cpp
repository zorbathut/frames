
#include <gtest/gtest.h>

#include <frames/frame.h>
#include <frames/mask.h>
#include <frames/raw.h>
#include <frames/sprite.h>
#include <frames/text.h>

#include "lib.h"

TEST(Layout, Background) {
  TestEnvironment env;

  Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
  frame->BackgroundSet(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  TestSnapshot(env);

  frame->BackgroundSet(Frames::Color(1.f, 1.f, 1.f, 1.0f));
  TestSnapshot(env);

  frame->BackgroundSet(Frames::Color(0.f, 1.f, 0.f, 0.2f));
  TestSnapshot(env);

  frame->BackgroundSet(Frames::Color(0.2f, 0.3f, 0.7f, 0.8f));
  TestSnapshot(env);
};

TEST(Layout, Pin) {
  TestEnvironment env;

  Frames::Frame *red = Frames::Frame::Create(env->RootGet(), "Test");
  red->BackgroundSet(Frames::Color(1.f, 0.f, 0.f, 0.5f)); // Partially transparent so we can see frame intersections

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

  Frames::Frame *green = Frames::Frame::Create(env->RootGet(), "Test");
  green->BackgroundSet(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  green->PinSet(Frames::CENTER, env->RootGet(), 0.75f, 0.75f);
  green->PinSet(Frames::TOPRIGHT, env->RootGet(), Frames::CENTERRIGHT);

  Frames::Frame *blue = Frames::Frame::Create(env->RootGet(), "Test");
  blue->BackgroundSet(Frames::Color(0.f, 0.f, 1.f, 0.5f));
  blue->PinSet(Frames::BOTTOMLEFT, env->RootGet(), Frames::CENTER);
  blue->PinSet(Frames::TOPRIGHT, env->RootGet(), Frames::TOPRIGHT);

  Frames::Frame *gray = Frames::Frame::Create(env->RootGet(), "Test");
  gray->BackgroundSet(Frames::Color(0.5f, 0.5f, 0.5f, 0.5f));
  gray->WidthSet((float)env.WidthGet() / 2);
  gray->HeightSet((float)env.HeightGet() / 2);
  gray->PinSet(Frames::CENTER, 0, Frames::Nil, Frames::Nil, (float)env.WidthGet() / 4, (float)env.HeightGet() / 4 * 3);

  {
    Frames::Layout::PinAxis axis = green->PinGet(Frames::X, 0.5);
    EXPECT_EQ(true, axis.valid);
    EXPECT_EQ(env->RootGet(), axis.target);
    EXPECT_EQ(0.75f, axis.point);
    EXPECT_EQ(0.f, axis.offset);
  }

  {
    Frames::Layout::PinAxis axis = green->PinGet(Frames::Y, 0.5);
    EXPECT_EQ(true, axis.valid);
    EXPECT_EQ(env->RootGet(), axis.target);
    EXPECT_EQ(0.75f, axis.point);
    EXPECT_EQ(0.f, axis.offset);
  }

  {
    Frames::Layout::PinPoint point = green->PinGet(Frames::CENTER);
    EXPECT_EQ(true, point.valid);
    EXPECT_EQ(env->RootGet(), point.target);
    EXPECT_EQ(Frames::Vector(0.75f, 0.75f), point.point);
    EXPECT_EQ(Frames::Vector(0.f, 0.f), point.offset);
  }

  {
    Frames::Layout::PinPoint point = green->PinGet(Frames::TOPRIGHT);
    EXPECT_EQ(true, point.valid);
    EXPECT_EQ(env->RootGet(), point.target);
    EXPECT_EQ(Frames::Vector(1.f, 0.5f), point.point);
    EXPECT_EQ(Frames::Vector(0.f, 0.f), point.offset);
  }

  {
    // a point we technically never set, but still one we can describe
    Frames::Layout::PinPoint point = green->PinGet(Frames::TOPCENTER);
    EXPECT_EQ(true, point.valid);
    EXPECT_EQ(env->RootGet(), point.target);
    EXPECT_EQ(Frames::Vector(0.75f, 0.5f), point.point);
    EXPECT_EQ(Frames::Vector(0.f, 0.f), point.offset);
  }

  {
    Frames::Layout::PinPoint point = green->PinGet(Frames::BOTTOMLEFT);
    EXPECT_EQ(false, point.valid);
  }

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

  {
    Frames::Layout::PinAxis axis = green->PinGet(Frames::X, 0);
    EXPECT_EQ(true, axis.valid);
    EXPECT_EQ(env->RootGet(), axis.target);
    EXPECT_EQ(0.f, axis.point);
    EXPECT_EQ(0.f, axis.offset);
  }

  {
    Frames::Layout::PinAxis axis = green->PinGet(Frames::X, 1);
    EXPECT_EQ(true, axis.valid);
    EXPECT_EQ(env->RootGet(), axis.target);
    EXPECT_EQ(1.f, axis.point);
    EXPECT_EQ(0.f, axis.offset);
  }

  {
    Frames::Layout::PinAxis axis = green->PinGet(Frames::Y, 0);
    EXPECT_EQ(true, axis.valid);
    EXPECT_EQ(red, axis.target);
    EXPECT_EQ(1.f, axis.point);
    EXPECT_EQ(0.f, axis.offset);
  }

  {
    Frames::Layout::PinAxis axis = green->PinGet(Frames::Y, 1);
    EXPECT_EQ(false, axis.valid);
  }

  {
    // the top part is valid; the right isn't
    Frames::Layout::PinPoint point = green->PinGet(Frames::TOPRIGHT);
    EXPECT_EQ(false, point.valid);
  }
};

TEST(Layout, Layer) {
  TestEnvironment env;

  const int testFrameCount = 20;
  Frames::Frame *frames[testFrameCount];
  const float order[testFrameCount] = { 0, 6, 3.14159f, 3, 5, 9, 12, -5, 0, 0, 3.14159f, 2, 2, 2, 5000, 4999, 5001, 0, 3, 3.14159f };  // Numbers are not magic in any way, just trying to provide an interesting cross-section
  {
    Frames::Layout *anchor = env->RootGet();
    for (int i = 0; i < testFrameCount; ++i) {
      frames[i] = Frames::Frame::Create(env->RootGet(), "Layer");
      frames[i]->WidthSet(400.f);
      frames[i]->HeightSet(400.f);
      frames[i]->BackgroundSet(Frames::Color((float)i / testFrameCount, (float)i / testFrameCount, (float)i / testFrameCount, 0.2f));
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
  env2.AllowErrors(); // due to how the test suite works, global errors will show up here (which is actually sorta convenient)

  Frames::Frame *subject = Frames::Frame::Create(env->RootGet(), "subject");
  Frames::Frame *subjectalt = Frames::Frame::Create(env->RootGet(), "subjectalt");
  Frames::Frame *subj2 = Frames::Frame::Create(env2->RootGet(), "subject2");

  // Forged enums
  subject->PinClear(Frames::Anchor(-1));
  subject->PinClear(Frames::Anchor(Frames::ANCHOR_COUNT));

  subject->PinClearAll(Frames::Axis(-1));
  subject->PinClearAll(Frames::Axis(2));

  subject->SizeClear(Frames::Axis(-1));
  subject->SizeClear(Frames::Axis(2));

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

  subject->PinGet(Frames::Axis(-1), 0);
  subject->PinGet(Frames::Axis(2), 0);

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
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::TOPLEFT, Frames::Nil, 1.0f);
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::TOPLEFT, 1.0f, Frames::Nil);
  subject->PinSet(Frames::TOPLEFT, subjectalt, Frames::TOPLEFT, Frames::Nil, Frames::Nil);

  subject->PinSet(Frames::LEFT, subjectalt, Frames::LEFT, Frames::Nil, 1.0f);
  subject->PinSet(Frames::LEFT, subjectalt, Frames::LEFT, Frames::Nil, Frames::Nil);
  subject->PinSet(Frames::LEFT, subjectalt, Frames::LEFT, 1.0f, 1.0f);

  subject->PinSet(Frames::TOP, subjectalt, Frames::TOP, 1.0f, Frames::Nil);
  subject->PinSet(Frames::TOP, subjectalt, Frames::TOP, Frames::Nil, Frames::Nil);
  subject->PinSet(Frames::TOP, subjectalt, Frames::TOP, 1.0f, 1.0f);

  EXPECT_EQ(0, Frames::Frame::Create(0, "Null"));
  EXPECT_EQ(0, Frames::Mask::Create(0, "Null"));
  EXPECT_EQ(0, Frames::Raw::Create(0, "Null"));
  EXPECT_EQ(0, Frames::Sprite::Create(0, "Null"));
  EXPECT_EQ(0, Frames::Text::Create(0, "Null"));
}

TEST(Layout, ChildGet) {
  TestEnvironment env;
  
  Frames::Frame *parent = Frames::Frame::Create(env->RootGet(), "parent");
  Frames::Frame *sibling = Frames::Frame::Create(env->RootGet(), "sibling");
  Frames::Frame *child = Frames::Frame::Create(parent, "child");
  Frames::Frame *implementation = Frames::Frame::Create(parent, "implementation");
  implementation->ImplementationSet(true);

  EXPECT_EQ(parent, env->RootGet()->ChildGetByName("parent"));
  EXPECT_EQ(sibling, env->RootGet()->ChildGetByName("sibling"));
  EXPECT_EQ(0, env->RootGet()->ChildGetByName("child"));
  EXPECT_EQ(0, env->RootGet()->ChildGetByName("invalid"));

  EXPECT_EQ(0, parent->ChildGetByName("parent"));
  EXPECT_EQ(0, parent->ChildGetByName("sibling"));
  EXPECT_EQ(child, parent->ChildGetByName("child"));
  EXPECT_EQ(0, parent->ChildGetByName("implementation"));
  EXPECT_EQ(0, parent->ChildGetByName("invalid"));

  EXPECT_EQ(0, parent->ChildImplementationGetByName("child"));
  EXPECT_EQ(implementation, parent->ChildImplementationGetByName("implementation"));
  EXPECT_EQ(0, parent->ChildImplementationGetByName("invalid"));
}