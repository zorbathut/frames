
#include <gtest/gtest.h>

#include <frames/frame.h>
#include <frames/mask.h>

#include "lib.h"

TEST(Mask, Basic) {
  TestEnvironment env;

  Frames::Mask *mask = Frames::Mask::CreateTagged(env->GetRoot());

  Frames::Frame *red = Frames::Frame::CreateTagged(mask);
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f)); // Partially transparent so we can see frame intersections
  red->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  red->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::CENTER);

  Frames::Frame *green = Frames::Frame::CreateTagged(mask);
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  green->SetPoint(Frames::CENTER, env->GetRoot(), 0.75f, 0.75f);
  green->SetPoint(Frames::TOPRIGHT, env->GetRoot(), Frames::CENTERRIGHT);

  Frames::Frame *blue = Frames::Frame::CreateTagged(mask);
  blue->SetBackground(Frames::Color(0.f, 0.f, 1.f, 0.5f));
  blue->SetPoint(Frames::BOTTOMLEFT, env->GetRoot(), Frames::CENTER);
  blue->SetPoint(Frames::TOPRIGHT, env->GetRoot(), Frames::TOPRIGHT);

  Frames::Frame *gray = Frames::Frame::CreateTagged(mask);
  gray->SetBackground(Frames::Color(0.5f, 0.5f, 0.5f, 0.5f));
  gray->SetWidth((float)env.GetWidth() / 2);
  gray->SetHeight((float)env.GetHeight() / 2);
  gray->SetPoint(Frames::CENTER, 0, Frames::Nil, Frames::Nil, (float)env.GetWidth() / 4, (float)env.GetHeight() / 4 * 3);

  mask->SetWidth(300);
  mask->SetHeight(200);
  mask->SetPoint(Frames::CENTER, env->GetRoot(), Frames::CENTER);

  TestSnapshot(env);
}

TEST(Mask, PixelPerfect) {
  TestEnvironment env;

  Frames::Mask *mask = Frames::Mask::CreateTagged(env->GetRoot());

  Frames::Frame *red = Frames::Frame::CreateTagged(mask);
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  red->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  red->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMRIGHT);
  
  Frames::Frame *green = Frames::Frame::CreateTagged(env->GetRoot());
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));

  mask->SetPoint(Frames::TOPLEFT, green, Frames::TOPLEFT);
  mask->SetPoint(Frames::BOTTOMRIGHT, green, Frames::BOTTOMRIGHT);
  
  green->SetPoint(Frames::CENTER, env->GetRoot(), Frames::CENTER);
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

  Frames::Frame *outerPre = Frames::Frame::CreateTagged(env->GetRoot());
  outerPre->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  outerPre->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::CENTER);
  outerPre->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  outerPre->SetLayer(1);

  Frames::Frame *outerPost = Frames::Frame::CreateTagged(env->GetRoot());
  outerPost->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::CENTER);
  outerPost->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMRIGHT);
  outerPost->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  outerPost->SetLayer(-1);

  Frames::Mask *outerMask = Frames::Mask::CreateTagged(env->GetRoot());
  outerMask->SetPoint(Frames::CENTER, env->GetRoot(), Frames::CENTER);
  outerMask->SetWidth(900);
  outerMask->SetHeight(600);

  Frames::Frame *middlePre = Frames::Frame::CreateTagged(outerMask);
  middlePre->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::CENTERLEFT);
  middlePre->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMCENTER);
  middlePre->SetBackground(Frames::Color(1.f, 0.f, 1.f, 0.5f));
  middlePre->SetLayer(1);

  Frames::Frame *middlePost = Frames::Frame::CreateTagged(outerMask);
  middlePost->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::TOPCENTER);
  middlePost->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::CENTERRIGHT);
  middlePost->SetBackground(Frames::Color(0.f, 1.f, 1.f, 0.5f));
  middlePost->SetLayer(-1);

  Frames::Mask *middleMask = Frames::Mask::CreateTagged(outerMask);
  middleMask->SetPoint(Frames::CENTER, env->GetRoot(), Frames::CENTER);
  middleMask->SetWidth(600);
  middleMask->SetHeight(400);

  Frames::Frame *inner = Frames::Frame::CreateTagged(middleMask);
  inner->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  inner->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMRIGHT);
  inner->SetBackground(Frames::Color(1.f, 1.f, 1.f, 0.5f));

  TestSnapshot(env);

  // Move the inner mask so that it isn't fully contained in the outer
  middleMask->SetPoint(Frames::CENTER, outerMask, Frames::CENTERRIGHT);

  TestSnapshot(env);

  // Move the inner mask so that it's fully outside the outer
  middleMask->SetPoint(Frames::CENTER, env->GetRoot(), Frames::CENTERRIGHT);
  middleMask->SetWidth(200);

  TestSnapshot(env);
}