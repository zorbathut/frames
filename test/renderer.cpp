
#include <gtest/gtest.h>

#include <frames/frame.h>
#include <frames/text.h>

#include "lib.h"

TEST(Renderer, Wrap) {
  TestEnvironment env;

  for (int x = 0; x < 256; ++x) {
    for (int y = 0; y < 256; ++y) {
      Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Color");
      frame->PinSet(Frames::TOPLEFT, env->RootGet(), x / 256.f, y / 256.f);
      frame->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), (x + 1) / 256.f, (y + 1) / 256.f);
      frame->BackgroundSet(Frames::Color(x / 255.f, 0.5, y / 255.f));
    }
  }

  TestSnapshot(env);
}

TEST(Renderer, DISABLED_Overflow) {
  TestEnvironment env;

  Frames::Text *text = Frames::Text::Create(env->RootGet(), "Text");
  text->WordwrapSet(true);
  std::string blob;
  for (int i = 0; i < 30000; ++i) {
    blob += 'a' + (i % 26);
  }
  text->TextSet(blob);
  text->SizeSet(10);
  text->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT);
  text->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::BOTTOMRIGHT);

  TestSnapshot(env);
}
