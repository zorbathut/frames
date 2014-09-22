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

TEST(Text, Sizing) {
  TestEnvironment env;

  // There was a problem with this font on one platform, though for some reason it is not reproducing. Whatever; test written, leaving it in.
  float sizes[] = {8, 12, 16, 24, 36, 48};
  for (int i = 0; i < sizeof(sizes) / sizeof(*sizes); ++i) {
    Frames::Text *tex = Frames::Text::Create(env->RootGet(), "Text");
    tex->FontSet("Sansation_Bold.ttf");
    tex->TextSet("300.0");
    tex->FontSizeSet(sizes[i]);
    tex->PinSet(Frames::CENTER, env->RootGet(), 0.5, (i + 0.5f) / (sizeof(sizes) / sizeof(*sizes)));
    tex->BackgroundSet(Frames::Color(0.2f, 0.2f, 0.2f));
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
