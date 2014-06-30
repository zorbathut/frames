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
#include <frames/detail_format.h>

#include "lib.h"

TEST(Helloworld, Helloworld) {
  TestEnvironment env(true, 640, 360);

  Frames::Frame *square = Frames::Frame::Create(env->RootGet(), "Square");
  square->BackgroundSet(Frames::Color(0.f, 0.f, 1.f));
  square->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER);
  
  TestSnapshot(env, SnapshotConfig().File("ref/doc/helloworld_hello"));
};

TEST(Helloworld, Setup) {
  TestEnvironment env(true, 640, 360);

  Frames::Frame *backdrop = Frames::Frame::Create(env->RootGet(), "Backdrop");
  Frames::Text *helloworld = Frames::Text::Create(backdrop, "Hello World");
  backdrop->BackgroundSet(Frames::Color(0.f, 0.f, 1.f));
  backdrop->PinSet(Frames::TOPLEFT, helloworld, Frames::TOPLEFT, -5, -5);
  backdrop->PinSet(Frames::BOTTOMRIGHT, helloworld, Frames::BOTTOMRIGHT, 5, 5);
  helloworld->FontSet("data/arial.ttf");
  helloworld->TextSet("Hello, World!");
  helloworld->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER);
  
  TestSnapshot(env, SnapshotConfig().File("ref/doc/helloworld_setup"));
};
