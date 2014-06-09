
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

TEST(Sprite, Rotation) {
  TestEnvironment env;
  const float span[] = { Frames::detail::Tau / 16, Frames::detail::Tau / 8, Frames::detail::Tau / 4, Frames::detail::Tau / 2, Frames::detail::Tau };
  const float sub = 8;
  const int spans = sizeof(span) / sizeof(*span);

  const char *const dudes[] = {
    "p1_front.png",
    "p2_front.png",
    "p3_front.png",
  };
  int dudeid = 0;

  for (int i = 0; i < sub + 1; ++i) {
    for (int j = 0; j < spans; ++j) {
      float yang = span[j] / sub * i;
      Frames::Sprite *a = Frames::Sprite::Create(env->RootGet(), "sprite");
      a->TextureSet(dudes[dudeid++]);
      dudeid %= sizeof(dudes) / sizeof(*dudes);
      a->PinSet(Frames::CENTER, env->RootGet(), (i + 0.5f) / (sub + 1), (j + 0.5f) / spans);
      a->EXPERIMENTAL_RotateSet(yang);
    }
  }

  // this one has always been flaky; the sampling differences on all the perfect-mip-zero rotated sprites is a frequent issue
  TestSnapshot(env, SnapshotConfig().Delta(3).Nearest(true));
}

TEST(Sprite, Sampling) {
  TestEnvironment env;

  Frames::Sprite *sprite = Frames::Sprite::Create(env->RootGet(), "Sprite");

  sprite->TextureSet("checkerboard.png");
  sprite->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER);

  TestSnapshot(env);
}

TEST(Sprite, Error) {
  TestEnvironment env;
  env.AllowErrors();

  Frames::Sprite *sprite = Frames::Sprite::Create(env->RootGet(), "Sprite");

  sprite->TextureSet("idontexist");

  TestSnapshot(env);  // this should be black
}
