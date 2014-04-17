
#include <gtest/gtest.h>

#include <frames/cast.h>
#include <frames/frame.h>
#include <frames/mask.h>
#include <frames/raw.h>
#include <frames/text.h>
#include <frames/texture.h>

#include "lib.h"

TEST(Core, Cast) {
  TestEnvironment env;

  Frames::Layout *layout = env->GetRoot();
  Frames::Layout *frame = Frames::Frame::Create("Frame", layout);
  Frames::Layout *mask = Frames::Mask::Create("Mask", layout);
  Frames::Layout *raw = Frames::Raw::Create("Raw", layout);
  Frames::Layout *text = Frames::Text::Create("Text", layout);
  Frames::Layout *texture = Frames::Texture::Create("Texture", layout);

  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(layout));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(frame));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(mask));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(raw));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(text));
  EXPECT_NE((Frames::Layout *)0, Frames::Cast<Frames::Layout>(texture));

  EXPECT_EQ((Frames::Frame *)0, Frames::Cast<Frames::Frame>(layout));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(frame));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(mask));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(raw));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(text));
  EXPECT_NE((Frames::Frame *)0, Frames::Cast<Frames::Frame>(texture));

  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(layout));
  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(frame));
  EXPECT_NE((Frames::Mask *)0, Frames::Cast<Frames::Mask>(mask));
  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(raw));
  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(text));
  EXPECT_EQ((Frames::Mask *)0, Frames::Cast<Frames::Mask>(texture));

  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(layout));
  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(frame));
  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(mask));
  EXPECT_NE((Frames::Raw *)0, Frames::Cast<Frames::Raw>(raw));
  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(text));
  EXPECT_EQ((Frames::Raw *)0, Frames::Cast<Frames::Raw>(texture));

  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(layout));
  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(frame));
  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(mask));
  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(raw));
  EXPECT_NE((Frames::Text *)0, Frames::Cast<Frames::Text>(text));
  EXPECT_EQ((Frames::Text *)0, Frames::Cast<Frames::Text>(texture));

  EXPECT_EQ((Frames::Texture *)0, Frames::Cast<Frames::Texture>(layout));
  EXPECT_EQ((Frames::Texture *)0, Frames::Cast<Frames::Texture>(frame));
  EXPECT_EQ((Frames::Texture *)0, Frames::Cast<Frames::Texture>(mask));
  EXPECT_EQ((Frames::Texture *)0, Frames::Cast<Frames::Texture>(raw));
  EXPECT_EQ((Frames::Texture *)0, Frames::Cast<Frames::Texture>(text));
  EXPECT_NE((Frames::Texture *)0, Frames::Cast<Frames::Texture>(texture));
};
