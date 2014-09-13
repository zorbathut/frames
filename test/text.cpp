/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

#include <gtest/gtest.h>

#include <frames/text.h>

#include "lib.h"

TEST(Text, Basic) {
  TestEnvironment env;

  const int c_tests = 7;

  const float sizes[c_tests] = { 12, 18, 24, 36, 48, 72, 96 };
  const char *text[c_tests] = {
    "Five hexing wizard bots jump quickly.",
    "Pack my box with five dozen liquor jugs.",
    "Waxy and quivering, jocks fumble the pizza.",
    "Both fickle dwarves jinx my pig quiz.",  // stupid dwarves, I liked that pig quiz
    "Cozy sphinx waves quart jug of bad milk.",
    "Jumpy halfling dwarves pick quartz box.", // and now my box! come on guys knock it off
    "The jay, pig, fox, zebra and my wolves quack!", // this is the worst guard wolf ever
  };
  Frames::Text *frames[c_tests] = { 0, };

  Frames::Frame *anchor = Frames::Frame::Create(env->RootGet(), "Test");
  anchor->PinSet(Frames::BOTTOMLEFT, env->RootGet(), Frames::TOPLEFT, 20.f, 0.f);
  for (int i = 0; i < c_tests; ++i) {
    Frames::Text *tex = Frames::Text::Create(env->RootGet(), "Layer");
    tex->FontSizeSet(sizes[i]);
    tex->TextSet(text[i]);
    tex->PinSet(Frames::TOPLEFT, anchor, Frames::BOTTOMLEFT, 0.f, 5.f);
    anchor = tex;
    frames[i] = tex;
  }

  TestSnapshot(env);

  for (int i = 0; i < c_tests; ++i) {
    if (i % 2 == 0) {
      frames[i]->FontSet("geo_1.ttf");
    }
  }

  TestSnapshot(env);

  for (int i = 0; i < c_tests; ++i) {
    frames[i]->WordwrapSet(true);
    frames[i]->PinSet(Frames::RIGHT, env->RootGet(), Frames::RIGHT, -20.f, Frames::Nil);
  }

  TestSnapshot(env);

  for (int i = 0; i < c_tests; ++i) {
    frames[i]->PinClear(Frames::RIGHT);
    frames[i]->WidthSet(sizes[i] * 7);
  }

  TestSnapshot(env);
}

TEST(Text, Error) {
  TestEnvironment env;
  env.AllowErrors(); // we'll have a bunch

  Frames::Text *tex = Frames::Text::Create(env->RootGet(), "Text");
  tex->FontSet("badfont.ttf");

  tex->TextSet("Generic text here");

  TestSnapshot(env); // should not crash
}

void InitStrokeChunk(std::vector<Frames::Text *> *out, TestEnvironment &env, const std::string &alpha, const std::string &beta, const std::string &gamma, bool right) {
  for (int i = 0; i < 3; ++i) {
    Frames::Text *label = Frames::Text::Create(env->RootGet(), "label");
    label->PinSet(Frames::BOTTOMCENTER, env->RootGet(), 0.25f + right * 0.5f, (i * 2 + 1) / 6.f, 0, -20);
    if (i == 0) {
      label->TextSet(alpha);
    } else if (i == 1) {
      label->TextSet(beta);
    } else if (i == 2) {
      label->TextSet(gamma);
    }
    label->FontSizeSet(24);
    label->ColorTextSet(Frames::Color(1, 1, 1));

    Frames::Text *target = Frames::Text::Create(env->RootGet(), "target");
    target->PinSet(Frames::TOPCENTER, env->RootGet(), 0.25f + right * 0.5f, (i * 2 + 1) / 6.f, 0, 20);
    target->TextSet("Zing, dwarf jocks vex lymph, Qutb.");
    target->FontSizeSet(80);
    target->EXPERIMENTAL_FillSet(false);
    target->EXPERIMENTAL_StrokeSet(true);
    target->EXPERIMENTAL_StrokeSizeSet(3.f);
    target->ColorTextSet(Frames::Color(1, 1, 0));
    if (right) {
      target->FontSet("geo_1.ttf");
    }

    out->push_back(target);
  }
}

std::vector<Frames::Text *> InitStrokeTest(TestEnvironment &env, const std::string &alpha, const std::string &beta, const std::string &gamma) {
  std::vector<Frames::Text *> result;
  InitStrokeChunk(&result, env, alpha, beta, gamma, false);
  InitStrokeChunk(&result, env, alpha, beta, gamma, true);
  return result;
}

TEST(Text, Stroke) {
  TestEnvironment env;

  std::vector<Frames::Text *> texen = InitStrokeTest(env, "Stroke", "None", "Both");

  // texen[0] already good
  texen[1]->EXPERIMENTAL_StrokeSet(false);
  texen[2]->EXPERIMENTAL_FillSet(true);
  // texen[3] already good
  texen[4]->EXPERIMENTAL_StrokeSet(false);
  texen[5]->EXPERIMENTAL_FillSet(true);

  TestSnapshot(env);
}

TEST(Text, StrokeSize) {
  TestEnvironment env;

  std::vector<Frames::Text *> texen = InitStrokeTest(env, "Stroke size 0.3", "Stroke size 1.0", "Stroke size 5.0");

  texen[0]->EXPERIMENTAL_StrokeSizeSet(0.3f);
  texen[1]->EXPERIMENTAL_StrokeSizeSet(1.f);
  texen[2]->EXPERIMENTAL_StrokeSizeSet(5.f);
  texen[3]->EXPERIMENTAL_StrokeSizeSet(0.3f);
  texen[4]->EXPERIMENTAL_StrokeSizeSet(1.f);
  texen[5]->EXPERIMENTAL_StrokeSizeSet(5.f);

  TestSnapshot(env);
}

TEST(Text, StrokeCap) {
  TestEnvironment env;

  std::vector<Frames::Text *> texen = InitStrokeTest(env, "Stroke cap Round", "Stroke cap Square", "Stroke cap Butt");

  texen[0]->EXPERIMENTAL_StrokeCapSet(Frames::FONTSTROKECAP_ROUND);
  texen[1]->EXPERIMENTAL_StrokeCapSet(Frames::FONTSTROKECAP_SQUARE);
  texen[2]->EXPERIMENTAL_StrokeCapSet(Frames::FONTSTROKECAP_BUTT);
  texen[3]->EXPERIMENTAL_StrokeCapSet(Frames::FONTSTROKECAP_ROUND);
  texen[4]->EXPERIMENTAL_StrokeCapSet(Frames::FONTSTROKECAP_SQUARE);
  texen[5]->EXPERIMENTAL_StrokeCapSet(Frames::FONTSTROKECAP_BUTT);

  TestSnapshot(env);
}

TEST(Text, StrokeJoin) {
  TestEnvironment env;

  std::vector<Frames::Text *> texen = InitStrokeTest(env, "Stroke join Round", "Stroke join Bevel", "Stroke join Miter");

  texen[0]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_ROUND);
  texen[1]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_BEVEL);
  texen[2]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_MITER);
  texen[3]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_ROUND);
  texen[4]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_BEVEL);
  texen[5]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_MITER);

  TestSnapshot(env);
}

TEST(Text, StrokeMiter) {
  TestEnvironment env;

  std::vector<Frames::Text *> texen = InitStrokeTest(env, "Miterlimit 0.3", "Miterlimit 1.0", "Miterlimit 5.0");

  texen[0]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_MITER);
  texen[1]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_MITER);
  texen[2]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_MITER);
  texen[3]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_MITER);
  texen[4]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_MITER);
  texen[5]->EXPERIMENTAL_StrokeJoinSet(Frames::FONTSTROKEJOIN_MITER);

  texen[0]->EXPERIMENTAL_StrokeMiterlimitSet(0.3f);
  texen[1]->EXPERIMENTAL_StrokeMiterlimitSet(1.f);
  texen[2]->EXPERIMENTAL_StrokeMiterlimitSet(5.f);
  texen[3]->EXPERIMENTAL_StrokeMiterlimitSet(0.3f);
  texen[4]->EXPERIMENTAL_StrokeMiterlimitSet(1.f);
  texen[5]->EXPERIMENTAL_StrokeMiterlimitSet(5.f);

  TestSnapshot(env);
}
