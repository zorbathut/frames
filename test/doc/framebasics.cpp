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
#include "doc/colors.h"

TEST(Framebasics, Example) {
  TestEnvironment env(true, 640, 360);

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(120);
    frame->HeightSet(70);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 50, 30);
    frame->BackgroundSet(tdc::red);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(50);
    frame->HeightSet(250);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 270, 50);
    frame->BackgroundSet(tdc::blue);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(110);
    frame->HeightSet(110);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 440, 60);
    frame->BackgroundSet(tdc::green);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(220);
    frame->HeightSet(180);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 30, 130);
    frame->BackgroundSet(tdc::cyan);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(120);
    frame->HeightSet(90);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 350, 200);
    frame->BackgroundSet(tdc::orange);
  }

  {
    Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Test");
    frame->WidthSet(90);
    frame->HeightSet(120);
    frame->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 490, 230);
    frame->BackgroundSet(tdc::purple);
  }

  TestSnapshot(env, SnapshotConfig().File("ref/doc/framebasics_example"));
};

void LayerMake(Frames::Layout *root, float x, float y, float layer, Frames::Color color, float text_y) {
  Frames::Frame *frame = Frames::Frame::Create(root, "layer");
  frame->WidthSet(80);
  frame->HeightSet(80);
  frame->PinSet(Frames::TOPLEFT, root, Frames::TOPLEFT, x, y);
  frame->BackgroundSet(color);
  frame->LayerSet(layer);

  Frames::Text *text = Frames::Text::Create(frame, "text");
  text->PinSet(0, text_y, frame, 0, text_y, 3, 3 * (1 - text_y * 2));
  text->SizeSet(12);
  text->FontSet("geo_1.ttf");
  text->TextSet(Frames::detail::Format("Layer %g", layer));
}

TEST(Framebasics, Layering) {
  TestEnvironment env(true, 640, 360);

  LayerMake(env->RootGet(), 40, 40, 0, tdc::blue, 0);
  LayerMake(env->RootGet(), 60, 60, 1, tdc::orange, 0);
  LayerMake(env->RootGet(), 80, 80, 2, tdc::red, 0);

  LayerMake(env->RootGet(), 220, 40, -1, tdc::purple, 0);
  LayerMake(env->RootGet(), 280, 60, 0, tdc::red, 0);
  LayerMake(env->RootGet(), 240, 80, 1, tdc::cyan, 0);

  LayerMake(env->RootGet(), 460, 80, 0, tdc::red, 1);
  LayerMake(env->RootGet(), 420, 60, 0.5, tdc::orange, 1);
  LayerMake(env->RootGet(), 440, 40, 20, tdc::green, 1);

  LayerMake(env->RootGet(), 40, 260, -1000, tdc::red, 1);
  LayerMake(env->RootGet(), 60, 240, -0.2f, tdc::orange, 1);
  LayerMake(env->RootGet(), 80, 220, 0.01f, tdc::green, 1);
  LayerMake(env->RootGet(), 100, 200, 0.015f, tdc::cyan, 1);

  LayerMake(env->RootGet(), 240, 200, 9000, tdc::cyan, 0);
  LayerMake(env->RootGet(), 220, 220, 9001, tdc::green, 0);
  LayerMake(env->RootGet(), 280, 240, 9002, tdc::purple, 0);
  LayerMake(env->RootGet(), 260, 260, 9003, tdc::blue, 0);

  LayerMake(env->RootGet(), 440, 260, 42, tdc::orange, 1);
  LayerMake(env->RootGet(), 460, 240, 113, tdc::red, 1);
  LayerMake(env->RootGet(), 420, 220, 1024, tdc::cyan, 1);
  LayerMake(env->RootGet(), 480, 200, 2600, tdc::purple, 1);

  TestSnapshot(env, SnapshotConfig().File("ref/doc/framebasics_layering"));
};

Frames::Frame *HierarchMake(Frames::Layout *root, Frames::Layout *parent, float x, float y, float layer, Frames::Color color, const std::string &intex) {
  Frames::Frame *frame = Frames::Frame::Create(parent, "layer");
  frame->WidthSet(200);
  frame->HeightSet(200);
  frame->PinSet(Frames::TOPLEFT, root, Frames::TOPLEFT, x, y);
  frame->BackgroundSet(color);
  frame->LayerSet(layer);

  Frames::Text *text = Frames::Text::Create(frame, "text");
  text->PinSet(Frames::TOPLEFT, frame, Frames::TOPLEFT, 3, 3);
  text->SizeSet(12);
  text->FontSet("geo_1.ttf");
  text->TextSet(Frames::detail::Format("%s\nLayer %g", intex, layer));

  return frame;
}

TEST(Framebasics, Hierarchy) {
  TestEnvironment env(true, 640, 360);

  Frames::Frame *rl = HierarchMake(env->RootGet(), env->RootGet(), 100, 40, 0, tdc::red, "RED");
  Frames::Frame *rr = HierarchMake(env->RootGet(), env->RootGet(), 340, 50, 1, tdc::blue, "BLUE");

  HierarchMake(env->RootGet(), rl, 130, 100, 100, tdc::orange, "ORANGE - Child of RED");
  HierarchMake(env->RootGet(), rr, 300, 140, 0, tdc::purple, "PURPLE - Child of BLUE");

  TestSnapshot(env, SnapshotConfig().File("ref/doc/framebasics_hierarchy"));
};
