
#include <gtest/gtest.h>

#include <frames/sprite.h>

#include "lib.h"

TEST(Sprite, Basic) {
  TestEnvironment env;

  Frames::Sprite *a = Frames::Sprite::Create(env->RootGet(), "a");
  Frames::Sprite *b = Frames::Sprite::Create(env->RootGet(), "b");
  Frames::Sprite *c = Frames::Sprite::Create(env->RootGet(), "c");

  a->TextureSet("p1_front.png");
  b->TextureSet("p2_front.png");
  c->TextureSet("p3_front.png");

  a->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER);
  b->PinSet(Frames::BOTTOMRIGHT, a, Frames::TOPLEFT);
  c->PinSet(Frames::TOPCENTER, a, Frames::BOTTOMCENTER);

  TestSnapshot(env);

  a->WidthSet(a->WidthGet() * 2);
  a->HeightSet(a->HeightGet() * 2);

  TestSnapshot(env);
}
