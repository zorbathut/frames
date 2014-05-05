
#include <gtest/gtest.h>

#include <frames/cast.h>
#include <frames/frame.h>
#include <frames/mask.h>
#include <frames/raw.h>
#include <frames/sprite.h>
#include <frames/text.h>

#include "lib.h"

TEST(Core, Cast) {
  TestEnvironment env;

  Frames::Layout *layout = env->RootGet();
  Frames::Layout *frame = Frames::Frame::Create(layout, "Frame");
  Frames::Layout *mask = Frames::Mask::Create(layout, "Mask");
  Frames::Layout *raw = Frames::Raw::Create(layout, "Raw");
  Frames::Layout *sprite = Frames::Sprite::Create(layout, "Sprite");
  Frames::Layout *text = Frames::Text::Create(layout, "Text");

  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(layout));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(frame));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(mask));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(raw));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(sprite));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(text));

  EXPECT_EQ((Frames::Frame *)0, Frames::Cast<Frames::Frame>(layout));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(frame));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(mask));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(raw));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(sprite));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(text));

  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(layout));
  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(frame));
  EXPECT_NE((Frames::Mask *)0, Frames::Cast<Frames::Mask>(mask));
  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(raw));
  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(sprite));
  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(text));

  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(layout));
  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(frame));
  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(mask));
  EXPECT_NE((Frames::Raw *)0, Frames::Cast<Frames::Raw>(raw));
  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(sprite));
  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(text));

  EXPECT_EQ((Frames::Sprite *)0, Frames::Cast<Frames::Sprite>(layout));
  EXPECT_EQ((Frames::Sprite *)0, Frames::Cast<Frames::Sprite>(frame));
  EXPECT_EQ((Frames::Sprite *)0, Frames::Cast<Frames::Sprite>(mask));
  EXPECT_EQ((Frames::Sprite *)0, Frames::Cast<Frames::Sprite>(raw));
  EXPECT_NE((Frames::Sprite *)0, Frames::Cast<Frames::Sprite>(sprite));
  EXPECT_EQ((Frames::Sprite *)0, Frames::Cast<Frames::Sprite>(text));

  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(layout));
  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(frame));
  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(mask));
  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(raw));
  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(sprite));
  EXPECT_NE((Frames::Text *)0, Frames::Cast<Frames::Text>(text));
};
