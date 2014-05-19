
#include <gtest/gtest.h>

#include <frames/cast.h>
#include <frames/detail_format.h>
#include <frames/frame.h>
#include <frames/mask.h>
#include <frames/raw.h>
#include <frames/sprite.h>
#include <frames/text.h>
#include <frames/texture.h>

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
}

TEST(Core, Format) {
  EXPECT_EQ("(0, 0)", Frames::detail::Format("%s", Frames::Vector(0, 0)));
  EXPECT_EQ("(1, 1)", Frames::detail::Format("%s", Frames::Vector(1, 1)));
  EXPECT_EQ("(-1, -1)", Frames::detail::Format("%s", Frames::Vector(-1, -1)));
  EXPECT_EQ("(0, 1)", Frames::detail::Format("%s", Frames::Vector(0, 1)));
  EXPECT_EQ("(0.5, 0)", Frames::detail::Format("%s", Frames::Vector(0.5f, 0)));
  EXPECT_EQ("(0, 0.1)", Frames::detail::Format("%s", Frames::Vector(0, 0.1f)));
  EXPECT_EQ("(-0.5, 0)", Frames::detail::Format("%s", Frames::Vector(-0.5f, 0)));
  EXPECT_EQ("(0, -0.1)", Frames::detail::Format("%s", Frames::Vector(0, -0.1f)));
}

TEST(Core, Const) {
  // Will always accept the canonical form
  EXPECT_EQ("TOPLEFT", Frames::DescriptorFromPoint(Frames::TOPLEFT));
  EXPECT_EQ("TOPLEFT", Frames::DescriptorFromPoint(Frames::LEFTTOP));
  EXPECT_EQ("TOPCENTER", Frames::DescriptorFromPoint(Frames::TOPCENTER));
  EXPECT_EQ("TOPCENTER", Frames::DescriptorFromPoint(Frames::CENTERTOP));
  EXPECT_EQ("TOPRIGHT", Frames::DescriptorFromPoint(Frames::TOPRIGHT));
  EXPECT_EQ("TOPRIGHT", Frames::DescriptorFromPoint(Frames::RIGHTTOP));
  EXPECT_EQ("CENTERLEFT", Frames::DescriptorFromPoint(Frames::CENTERLEFT));
  EXPECT_EQ("CENTERLEFT", Frames::DescriptorFromPoint(Frames::LEFTCENTER));
  EXPECT_EQ("CENTER", Frames::DescriptorFromPoint(Frames::CENTER));
  EXPECT_EQ("CENTER", Frames::DescriptorFromPoint(Frames::CENTERCENTER));
  EXPECT_EQ("CENTERRIGHT", Frames::DescriptorFromPoint(Frames::CENTERRIGHT));
  EXPECT_EQ("CENTERRIGHT", Frames::DescriptorFromPoint(Frames::RIGHTCENTER));
  EXPECT_EQ("BOTTOMLEFT", Frames::DescriptorFromPoint(Frames::BOTTOMLEFT));
  EXPECT_EQ("BOTTOMLEFT", Frames::DescriptorFromPoint(Frames::LEFTBOTTOM));
  EXPECT_EQ("BOTTOMCENTER", Frames::DescriptorFromPoint(Frames::BOTTOMCENTER));
  EXPECT_EQ("BOTTOMCENTER", Frames::DescriptorFromPoint(Frames::CENTERBOTTOM));
  EXPECT_EQ("BOTTOMRIGHT", Frames::DescriptorFromPoint(Frames::BOTTOMRIGHT));
  EXPECT_EQ("BOTTOMRIGHT", Frames::DescriptorFromPoint(Frames::RIGHTBOTTOM));
  EXPECT_EQ("LEFT", Frames::DescriptorFromPoint(Frames::LEFT));
  EXPECT_EQ("CENTERX", Frames::DescriptorFromPoint(Frames::CENTERX));
  EXPECT_EQ("RIGHT", Frames::DescriptorFromPoint(Frames::RIGHT));
  EXPECT_EQ("TOP", Frames::DescriptorFromPoint(Frames::TOP));
  EXPECT_EQ("CENTERY", Frames::DescriptorFromPoint(Frames::CENTERY));
  EXPECT_EQ("BOTTOM", Frames::DescriptorFromPoint(Frames::BOTTOM));
}

TEST(Core, Error) {
  {
    Frames::ConfigurationGlobal cg;
    Frames::Ptr<TestLogger> log(new TestLogger());
    cg.LoggerSet(log);
    log->AllowErrors();
    Frames::ConfigurationGlobalSet(cg);
  }

  EXPECT_EQ("", Frames::DescriptorFromPoint(Frames::Anchor(-1)));
  EXPECT_EQ("", Frames::DescriptorFromPoint(Frames::ANCHOR_COUNT));
  EXPECT_EQ(Frames::Vector(0.f, 0.f), Frames::PointFromAnchor(Frames::Anchor(-1)));
  EXPECT_EQ(Frames::Vector(0.f, 0.f), Frames::PointFromAnchor(Frames::ANCHOR_COUNT));

  Frames::Input::StringFromKey(Frames::Input::INVALID);
  Frames::Input::StringFromKey(Frames::Input::Key(-1));

  Frames::Input::Command cmd;
  EXPECT_EQ(-1, cmd.MouseDownButtonGet());
  EXPECT_EQ(-1, cmd.MouseUpButtonGet());
  EXPECT_EQ(0, cmd.MouseWheelDeltaGet());
  EXPECT_EQ(0, cmd.MouseMoveXGet());
  EXPECT_EQ(0, cmd.MouseMoveYGet());
  cmd.MetaGet();
  EXPECT_EQ(Frames::Input::INVALID, cmd.KeyDownGet());
  EXPECT_EQ(Frames::Input::INVALID, cmd.KeyUpGet());
  EXPECT_EQ(Frames::Input::INVALID, cmd.KeyRepeatGet());
  EXPECT_EQ("", cmd.KeyTextGet());

  EXPECT_EQ(4, Frames::Texture::RawBPPGet(Frames::Texture::Format(-1)));
  EXPECT_EQ(4, Frames::Texture::RawBPPGet(Frames::Texture::FORMAT_COUNT));

  Frames::ConfigurationGlobalSet(Frames::ConfigurationGlobal());  // force it out of scope
}
