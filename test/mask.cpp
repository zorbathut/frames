
#include <gtest/gtest.h>

#include <frames/frame.h>
#include <frames/mask.h>

#include "lib.h"

TEST(Mask, Basic) {
  TestEnvironment env;

  Frames::Mask *mask = Frames::Mask::Create("Test", env->GetRoot());

  Frames::Frame *red = Frames::Frame::Create("Red", mask);
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f)); // Partially transparent so we can see frame intersections
  red->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  red->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::CENTER);

  Frames::Frame *green = Frames::Frame::Create("Green", mask);
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  green->SetPin(Frames::CENTER, env->GetRoot(), 0.75f, 0.75f);
  green->SetPin(Frames::TOPRIGHT, env->GetRoot(), Frames::CENTERRIGHT);

  Frames::Frame *blue = Frames::Frame::Create("blue", mask);
  blue->SetBackground(Frames::Color(0.f, 0.f, 1.f, 0.5f));
  blue->SetPin(Frames::BOTTOMLEFT, env->GetRoot(), Frames::CENTER);
  blue->SetPin(Frames::TOPRIGHT, env->GetRoot(), Frames::TOPRIGHT);

  Frames::Frame *gray = Frames::Frame::Create("gray", mask);
  gray->SetBackground(Frames::Color(0.5f, 0.5f, 0.5f, 0.5f));
  gray->SetWidth((float)env.GetWidth() / 2);
  gray->SetHeight((float)env.GetHeight() / 2);
  gray->SetPin(Frames::CENTER, 0, Frames::Nil, Frames::Nil, (float)env.GetWidth() / 4, (float)env.GetHeight() / 4 * 3);

  mask->SetWidth(300);
  mask->SetHeight(200);
  mask->SetPin(Frames::CENTER, env->GetRoot(), Frames::CENTER);

  TestSnapshot(env);
}

TEST(Mask, PixelPerfect) {
  TestEnvironment env;

  Frames::Mask *mask = Frames::Mask::Create("Test", env->GetRoot());

  Frames::Frame *red = Frames::Frame::Create("Red", mask);
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  red->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  red->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMRIGHT);
  
  Frames::Frame *green = Frames::Frame::Create("Green", env->GetRoot());
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));

  mask->SetPin(Frames::TOPLEFT, green, Frames::TOPLEFT);
  mask->SetPin(Frames::BOTTOMRIGHT, green, Frames::BOTTOMRIGHT);
  
  green->SetPin(Frames::CENTER, env->GetRoot(), Frames::CENTER);
  green->SetWidth(400);
  green->SetHeight(300);

  TestSnapshot(env);
};

TEST(Mask, Nested) {
  TestEnvironment env;

  // Hierarchy:
  // Frame outerPre
  // Mask outerMask
  //  Frame middlePre
  //  Mask middleMask
  //    Frame inner
  //  Frame middlePost
  // Frame outerPost

  Frames::Frame *outerPre = Frames::Frame::Create("Test", env->GetRoot());
  outerPre->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  outerPre->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::CENTER);
  outerPre->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  outerPre->SetLayer(1);

  Frames::Frame *outerPost = Frames::Frame::Create("Test", env->GetRoot());
  outerPost->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::CENTER);
  outerPost->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMRIGHT);
  outerPost->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  outerPost->SetLayer(-1);

  Frames::Mask *outerMask = Frames::Mask::Create("Test", env->GetRoot());
  outerMask->SetPin(Frames::CENTER, env->GetRoot(), Frames::CENTER);
  outerMask->SetWidth(900);
  outerMask->SetHeight(600);

  Frames::Frame *middlePre = Frames::Frame::Create("Test", outerMask);
  middlePre->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::CENTERLEFT);
  middlePre->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMCENTER);
  middlePre->SetBackground(Frames::Color(1.f, 0.f, 1.f, 0.5f));
  middlePre->SetLayer(1);

  Frames::Frame *middlePost = Frames::Frame::Create("Test", outerMask);
  middlePost->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPCENTER);
  middlePost->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::CENTERRIGHT);
  middlePost->SetBackground(Frames::Color(0.f, 1.f, 1.f, 0.5f));
  middlePost->SetLayer(-1);

  Frames::Mask *middleMask = Frames::Mask::Create("Test", outerMask);
  middleMask->SetPin(Frames::CENTER, env->GetRoot(), Frames::CENTER);
  middleMask->SetWidth(600);
  middleMask->SetHeight(400);

  Frames::Frame *inner = Frames::Frame::Create("Test", middleMask);
  inner->SetPin(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  inner->SetPin(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMRIGHT);
  inner->SetBackground(Frames::Color(1.f, 1.f, 1.f, 0.5f));

  TestSnapshot(env);

  // Move the inner mask so that it isn't fully contained in the outer
  middleMask->SetPin(Frames::CENTER, outerMask, Frames::CENTERRIGHT);

  TestSnapshot(env);

  // Move the inner mask so that it's fully outside the outer
  middleMask->SetPin(Frames::CENTER, env->GetRoot(), Frames::CENTERRIGHT);
  middleMask->SetWidth(200);

  TestSnapshot(env);
}