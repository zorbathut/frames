
#include <gtest/gtest.h>

#include <frames/frame.h>
#include <frames/mask.h>

#include "lib.h"

TEST(Mask, Basic) {
  TestEnvironment env;

  Frames::Mask *mask = Frames::Mask::Create("Test", env->RootGet());

  Frames::Frame *red = Frames::Frame::Create("Red", mask);
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f)); // Partially transparent so we can see frame intersections
  red->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT);
  red->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::CENTER);

  Frames::Frame *green = Frames::Frame::Create("Green", mask);
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  green->PinSet(Frames::CENTER, env->RootGet(), 0.75f, 0.75f);
  green->PinSet(Frames::TOPRIGHT, env->RootGet(), Frames::CENTERRIGHT);

  Frames::Frame *blue = Frames::Frame::Create("blue", mask);
  blue->SetBackground(Frames::Color(0.f, 0.f, 1.f, 0.5f));
  blue->PinSet(Frames::BOTTOMLEFT, env->RootGet(), Frames::CENTER);
  blue->PinSet(Frames::TOPRIGHT, env->RootGet(), Frames::TOPRIGHT);

  Frames::Frame *gray = Frames::Frame::Create("gray", mask);
  gray->SetBackground(Frames::Color(0.5f, 0.5f, 0.5f, 0.5f));
  gray->WidthSet((float)env.WidthGet() / 2);
  gray->HeightSet((float)env.HeightGet() / 2);
  gray->PinSet(Frames::CENTER, 0, Frames::Nil, Frames::Nil, (float)env.WidthGet() / 4, (float)env.HeightGet() / 4 * 3);

  mask->WidthSet(300);
  mask->HeightSet(200);
  mask->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER);

  TestSnapshot(env);
}

TEST(Mask, PixelPerfect) {
  TestEnvironment env;

  Frames::Mask *mask = Frames::Mask::Create("Test", env->RootGet());

  Frames::Frame *red = Frames::Frame::Create("Red", mask);
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  red->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT);
  red->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::BOTTOMRIGHT);
  
  Frames::Frame *green = Frames::Frame::Create("Green", env->RootGet());
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));

  mask->PinSet(Frames::TOPLEFT, green, Frames::TOPLEFT);
  mask->PinSet(Frames::BOTTOMRIGHT, green, Frames::BOTTOMRIGHT);
  
  green->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER);
  green->WidthSet(400);
  green->HeightSet(300);

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

  Frames::Frame *outerPre = Frames::Frame::Create("Test", env->RootGet());
  outerPre->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT);
  outerPre->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::CENTER);
  outerPre->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  outerPre->LayerSet(1);

  Frames::Frame *outerPost = Frames::Frame::Create("Test", env->RootGet());
  outerPost->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::CENTER);
  outerPost->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::BOTTOMRIGHT);
  outerPost->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  outerPost->LayerSet(-1);

  Frames::Mask *outerMask = Frames::Mask::Create("Test", env->RootGet());
  outerMask->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER);
  outerMask->WidthSet(900);
  outerMask->HeightSet(600);

  Frames::Frame *middlePre = Frames::Frame::Create("Test", outerMask);
  middlePre->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::CENTERLEFT);
  middlePre->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::BOTTOMCENTER);
  middlePre->SetBackground(Frames::Color(1.f, 0.f, 1.f, 0.5f));
  middlePre->LayerSet(1);

  Frames::Frame *middlePost = Frames::Frame::Create("Test", outerMask);
  middlePost->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPCENTER);
  middlePost->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::CENTERRIGHT);
  middlePost->SetBackground(Frames::Color(0.f, 1.f, 1.f, 0.5f));
  middlePost->LayerSet(-1);

  Frames::Mask *middleMask = Frames::Mask::Create("Test", outerMask);
  middleMask->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER);
  middleMask->WidthSet(600);
  middleMask->HeightSet(400);

  Frames::Frame *inner = Frames::Frame::Create("Test", middleMask);
  inner->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT);
  inner->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::BOTTOMRIGHT);
  inner->SetBackground(Frames::Color(1.f, 1.f, 1.f, 0.5f));

  TestSnapshot(env);

  // Move the inner mask so that it isn't fully contained in the outer
  middleMask->PinSet(Frames::CENTER, outerMask, Frames::CENTERRIGHT);

  TestSnapshot(env);

  // Move the inner mask so that it's fully outside the outer
  middleMask->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTERRIGHT);
  middleMask->WidthSet(200);

  TestSnapshot(env);
}