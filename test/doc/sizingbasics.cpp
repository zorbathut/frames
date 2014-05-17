
#include <gtest/gtest.h>

#include <frames/cast.h>
#include <frames/frame.h>
#include <frames/sprite.h>
#include <frames/text.h>
#include <frames/detail_format.h>

#include "lib.h"
#include "doclib.h"
#include "doc/colors.h"

TEST(Sizingbasics, Default) {
  TestEnvironment env(true, 640, 360);

  Frames::Frame *a = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *b = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *c = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *d = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *e = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *f = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *g = Frames::Frame::Create(env->RootGet(), "Example");

  a->BackgroundSet(tdc::red);
  b->BackgroundSet(tdc::green);
  c->BackgroundSet(tdc::blue);
  d->BackgroundSet(tdc::orange);
  e->BackgroundSet(tdc::cyan);
  f->BackgroundSet(tdc::purple);
  g->BackgroundSet(tdc::white);

  a->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER, -230, -104);
  b->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER, 119, -66);
  c->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER, 12, 155);
  d->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER, -70, 120);
  e->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER, 283, 30);
  f->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER, -140, 70);
  g->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER, 0, 0);

  CreateNameBar(env->RootGet(), "Default frame size");

  TestSnapshot(env, "ref/doc/sizingbasics_default");

}