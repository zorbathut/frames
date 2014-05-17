
#include <gtest/gtest.h>

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

TEST(Sizingbasics, Resize) {
  TestEnvironment env(true, 640, 360);

  CreateNameBar(env->RootGet(), "Self-sized Sprite and Text");

  Frames::Sprite *sprite1 = Frames::Sprite::Create(env->RootGet(), "Example");
  Frames::Sprite *sprite2 = Frames::Sprite::Create(env->RootGet(), "Example");
  Frames::Sprite *sprite3 = Frames::Sprite::Create(env->RootGet(), "Example");
  Frames::Sprite *sprite4 = Frames::Sprite::Create(env->RootGet(), "Example");
  Frames::Text *text1 = Frames::Text::Create(env->RootGet(), "Example");
  Frames::Text *text2 = Frames::Text::Create(env->RootGet(), "Example");
  Frames::Text *text3 = Frames::Text::Create(env->RootGet(), "Example");

  sprite1->BackgroundSet(tdc::red);
  sprite2->BackgroundSet(tdc::green);
  sprite3->BackgroundSet(tdc::blue);
  // we leave sprite4 transparent intentionally
  text1->BackgroundSet(tdc::orange);
  text2->BackgroundSet(tdc::cyan);
  text3->BackgroundSet(tdc::purple);

  sprite1->TextureSet("Explosion.png");
  sprite2->TextureSet("mute.png");
  sprite3->TextureSet("pause.png");
  sprite4->TextureSet("makhana.png");

  text1->TextSet("The quick brown fox jumps over the lazy dog.");
  text2->TextSet("The gostak distims the doshes.");
  text3->TextSet("'Twas brillig, and the slithy toves\nDid gyre and gimble in the wabe.");

  text1->SizeSet(20);
  text2->SizeSet(30);
  text3->FontSet("geo_1.ttf");
  text3->SizeSet(12);

  sprite1->PinSet(Frames::CENTERBOTTOM, env->RootGet(), Frames::CENTERBOTTOM, -120, 20);
  sprite2->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 10, 10);
  sprite3->PinSet(Frames::TOPLEFT, sprite2, Frames::BOTTOMLEFT);
  sprite4->PinSet(Frames::CENTER, env->RootGet(), 0.2f, 0.4f);

  text1->PinSet(Frames::CENTER, env->RootGet(), 0.75, 0.25);
  text2->PinSet(Frames::CENTER, env->RootGet(), 0.75, 0.5);
  text3->PinSet(Frames::CENTER, env->RootGet(), 0.75, 0.75);

  TestSnapshot(env, "ref/doc/sizingbasics_self");
}

TEST(Sizingbasics, SpriteResize) {
  TestEnvironment env(true, 640, 360);

  CreateNameBar(env->RootGet(), "Resized Sprite");

  Frames::Sprite *sprite1 = Frames::Sprite::Create(env->RootGet(), "Example");
  Frames::Sprite *sprite2 = Frames::Sprite::Create(env->RootGet(), "Example");
  Frames::Sprite *sprite3 = Frames::Sprite::Create(env->RootGet(), "Example");

  sprite1->BackgroundSet(tdc::red);
  sprite2->BackgroundSet(tdc::red);
  sprite3->BackgroundSet(tdc::red);

  sprite1->TextureSet("p1_front.png");
  sprite2->TextureSet("p1_front.png");
  sprite3->TextureSet("p1_front.png");

  sprite1->PinSet(Frames::CENTER, env->RootGet(), 1.f / 6, 0.6f);
  sprite2->PinSet(Frames::CENTER, env->RootGet(), 3.f / 6, 0.6f);
  sprite3->PinSet(Frames::CENTER, env->RootGet(), 5.f / 6, 0.6f);

  sprite2->WidthSet(66 * 2);
  sprite2->HeightSet(92 * 2);

  sprite3->WidthSet(66 * 2);

  Annotate(sprite1, "Default size");
  Annotate(sprite2, "Aspect ratio preserved");
  Annotate(sprite3, "Aspect ratio ignored");

  TestSnapshot(env, "ref/doc/sizingbasics_sprite");
}

TEST(Sizingbasics, TextResize) {
  TestEnvironment env(true, 640, 360);

  CreateNameBar(env->RootGet(), "Resized Text");

  // default
  // width explicit, no wordwrap
  // width explicit, height explicit, no wordwrap

  // width explicit with wordwrap
  // width explicit with wordwrap, height increased
  // width explicit with wordwrap, height reduced

  Frames::Text *text1 = Frames::Text::Create(env->RootGet(), "Example");
  Frames::Text *text2 = Frames::Text::Create(env->RootGet(), "Example");
  Frames::Text *text3 = Frames::Text::Create(env->RootGet(), "Example");
  Frames::Text *text4 = Frames::Text::Create(env->RootGet(), "Example");
  Frames::Text *text5 = Frames::Text::Create(env->RootGet(), "Example");
  Frames::Text *text6 = Frames::Text::Create(env->RootGet(), "Example");

  text1->BackgroundSet(tdc::red);
  text2->BackgroundSet(tdc::red);
  text3->BackgroundSet(tdc::red);
  text4->BackgroundSet(tdc::red);
  text5->BackgroundSet(tdc::red);
  text6->BackgroundSet(tdc::red);

  text1->SizeSet(16);
  text2->SizeSet(16);
  text3->SizeSet(16);
  text4->SizeSet(16);
  text5->SizeSet(16);
  text6->SizeSet(16);

  text1->TextSet("West of Arkham the hills rise wild.");
  text2->TextSet("West of Arkham the hills rise wild.");
  text3->TextSet("West of Arkham the hills rise wild.");
  text4->TextSet("West of Arkham the hills rise wild.");
  text5->TextSet("West of Arkham the hills rise wild.");
  text6->TextSet("West of Arkham the hills rise wild.");

  text1->PinSet(Frames::CENTER, env->RootGet(), 1.f / 6, 2.f / 5);
  text2->PinSet(Frames::CENTER, env->RootGet(), 3.f / 6, 2.f / 5);
  text3->PinSet(Frames::CENTER, env->RootGet(), 5.f / 6, 2.f / 5);
  text4->PinSet(Frames::CENTER, env->RootGet(), 1.f / 6, 4.f / 5);
  text5->PinSet(Frames::CENTER, env->RootGet(), 3.f / 6, 4.f / 5);
  text6->PinSet(Frames::CENTER, env->RootGet(), 5.f / 6, 4.f / 5);

  Annotate(text1, "Default size");

  text2->WidthSet(115);
  Annotate(text2, "Width reduced, height default");

  text3->WidthSet(115);
  text3->HeightSet(8);
  Annotate(text3, "Width/height reduced");

  text4->WordwrapSet(true);
  text4->WidthSet(115);
  Annotate(text4, "Wordwrap enabled", "Width reduced, height default");

  text5->WordwrapSet(true);
  text5->WidthSet(115);
  text5->HeightSet(50);
  Annotate(text5, "Wordwrap enabled", "Width reduced, height increased");

  text6->WordwrapSet(true);
  text6->WidthSet(115);
  text6->HeightSet(22);
  Annotate(text6, "Wordwrap enabled", "Width/height reduced");

  TestSnapshot(env, "ref/doc/sizingbasics_text");
}
