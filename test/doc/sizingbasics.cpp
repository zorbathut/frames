
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

  CreateNameBar(env->RootGet(), "Default frame size");

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

  TestSnapshot(env, "ref/doc/sizingbasics_default");
}

void Annotate(Frames::Frame *frame, const std::string &tex1, const std::string &tex2 = "") {
  Frames::Text *btex = 0;
  if (!tex2.empty()) {
    btex = Frames::Text::Create(frame, "Annotate");

    btex->TextSet(tex2);
    btex->PinSet(Frames::CENTERBOTTOM, frame, Frames::CENTERTOP, 0, -10);
    btex->ColorTextSet(Frames::Color(1.f, 1.f, 0.f));
    btex->FontSet("geo_1.ttf");
  }

  Frames::Text *text = Frames::Text::Create(frame, "Annotate");

  text->TextSet(tex1);
  text->PinSet(Frames::CENTERBOTTOM, btex ? btex : frame, Frames::CENTERTOP, 0, btex ? 0 : -10.f);
  text->ColorTextSet(Frames::Color(1.f, 1.f, 0.f));
  text->FontSet("geo_1.ttf");
}

TEST(Sizingbasics, Simple) {
  TestEnvironment env(true, 640, 360);

  CreateNameBar(env->RootGet(), "Resized frames");

  Frames::Frame *a = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *b = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *c = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *d = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *e = Frames::Frame::Create(env->RootGet(), "Example");
  Frames::Frame *f = Frames::Frame::Create(env->RootGet(), "Example");

  a->BackgroundSet(tdc::red);
  b->BackgroundSet(tdc::green);
  c->BackgroundSet(tdc::blue);
  d->BackgroundSet(tdc::orange);
  e->BackgroundSet(tdc::cyan);
  f->BackgroundSet(tdc::purple);

  a->PinSet(Frames::CENTER, env->RootGet(), 1.f/6, 2.f/5);
  b->PinSet(Frames::CENTER, env->RootGet(), 3.f/6, 2.f/5);
  c->PinSet(Frames::CENTER, env->RootGet(), 5.f/6, 2.f/5);
  d->PinSet(Frames::CENTER, env->RootGet(), 1.f/6, 4.f/5);
  e->PinSet(Frames::CENTER, env->RootGet(), 3.f/6, 4.f/5);
  f->PinSet(Frames::CENTER, env->RootGet(), 5.f/6, 4.f/5);

  Annotate(a, "Default size");

  b->WidthSet(80);
  Annotate(b, "WidthSet(80)");
  
  c->HeightSet(80);
  Annotate(c, "HeightSet(80)");

  d->WidthSet(80);
  d->HeightSet(80);
  Annotate(d, "WidthSet(80)", "HeightSet(80)");

  e->WidthSet(80);
  e->HeightSet(10);
  Annotate(e, "WidthSet(80)", "HeightSet(10)");

  f->WidthSet(10);
  f->HeightSet(80);
  Annotate(f, "WidthSet(10)", "HeightSet(80)");
  
  TestSnapshot(env, "ref/doc/sizingbasics_simple");
}