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
  text->FontSizeSet(10);
  text->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT);
  text->PinSet(Frames::BOTTOMRIGHT, env->RootGet(), Frames::BOTTOMRIGHT);

  TestSnapshot(env);
}
