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

#include <frames/cast.h>
#include <frames/frame.h>
#include <frames/sprite.h>
#include <frames/text.h>
#include <frames/detail_format.h>

#include "lib.h"
#include "doclib.h"
#include "doc/colors.h"

class Arrow : public Frames::Frame {
  FRAMES_DECLARE_RTTI();
public:
  static Arrow *Create(Frames::Layout *parent, const std::string &name) {
    return new Arrow(parent, name);
  }

private:
  Arrow(Frames::Layout *parent, const std::string &name) : Frame(parent, name) {
    m_head = Frames::Sprite::Create(this, "Head");
    m_body = Frames::Sprite::Create(this, "Body");
    m_reticle = Frames::Sprite::Create(this, "Reticle");

    m_head->TextureSet("linehead.png");
    m_body->TextureSet("linebody.png");
    m_reticle->TextureSet("sqtarget.png");

    m_head->ImplementationSet(true);
    m_body->ImplementationSet(true);
    m_reticle->ImplementationSet(true);

    m_head->WidthSet(m_head->WidthGet());

    m_body->PinSet(Frames::CENTER, this, Frames::CENTER);
    m_reticle->PinSet(Frames::CENTER, this, Frames::CENTER);

    EventAttach(Frames::Layout::Event::Size, Frames::Delegate<void(Frames::Handle*)>(this, &Arrow::Reorient));
  }

  void Reorient(Frames::Handle *handle) {
    if (WidthGet() || HeightGet()) {
      m_head->VisibleSet(true);
      m_body->VisibleSet(true);
      m_reticle->VisibleSet(false);
      float ang = -atan2(WidthGet(), HeightGet());
      float dist = sqrt(WidthGet() * WidthGet() + HeightGet() * HeightGet());
      m_head->EXPERIMENTAL_RotateSet(ang);
      m_body->EXPERIMENTAL_RotateSet(ang);
      m_head->PinSet(Frames::CENTER, this, 1 - m_head->HeightGet() / dist / 2, 1 - m_head->HeightGet() / dist / 2);
      m_body->HeightSet(dist);
    } else {
      m_head->VisibleSet(false);
      m_body->VisibleSet(false);
      m_reticle->VisibleSet(true);
    }
  }

  Frames::Sprite *m_head;
  Frames::Sprite *m_body;
  Frames::Sprite *m_reticle;
};
FRAMES_DEFINE_RTTI(Arrow, Frames::Frame);

void AddDebugDisplay(Frames::Layout *root, Frames::Layout *target, Frames::Anchor point, Frames::Anchor texanchor = Frames::CENTERLEFT, float tdx = 7, float tdy = 0, bool newl = false) {
  Frames::Frame *arrows = root->ChildGetByName("Arrows");
  if (!arrows) {
    arrows = Frames::Frame::Create(root, "Arrows");
    arrows->LayerSet(1000); // should be high enough
  }

  Frames::Layout::PinPoint ppt = target->PinGet(point);

  if (!ppt.target) {
    // what
    FAIL();
    return;
  }

  Arrow *arrow = Arrow::Create(arrows, "Arrow");
  arrow->PinSet(Frames::TOPLEFT, target, point);
  arrow->PinSet(Frames::BOTTOMRIGHT, ppt.target, ppt.point.x, ppt.point.y);

  Frames::Text *tex = Frames::Text::Create(arrows, "ArrowText");
  tex->TextSet(Frames::detail::Format("Pin from %s.%s to %s.%s%swith an offset of %s", target->NameGet(), Frames::DescriptorFromPoint(point), ppt.target->NameGet(), Frames::DescriptorFromPoint(ppt.point.x, ppt.point.y), newl ? "\n" : " ", ppt.offset));
  tex->PinSet(texanchor, arrow, Frames::CENTER, tdx, tdy);
  tex->ColorTextSet(Frames::Color(1.f, 1.f, 0.f));
  tex->FontSet("geo_1.ttf");
}

TEST(Pinningbasics, Example) {
  TestEnvironment env(true, 640, 360);

  Frames::Frame *health = Frames::Frame::Create(env->RootGet(), "Health");
  Frames::Frame *energy = Frames::Frame::Create(env->RootGet(), "Energy");
  Frames::Frame *stamina = Frames::Frame::Create(env->RootGet(), "Stamina");

  health->HeightSet(30);
  energy->HeightSet(30);
  stamina->HeightSet(30);

  health->BackgroundSet(tdc::redDark);
  energy->BackgroundSet(tdc::greenDark);
  stamina->BackgroundSet(tdc::blueDark);

  health->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 40, 40);
  energy->PinSet(Frames::TOPLEFT, health, Frames::BOTTOMLEFT, 0, 20);
  stamina->PinSet(Frames::TOPLEFT, energy, Frames::BOTTOMLEFT, 0, 20);

  health->WidthSet(200);
  energy->PinSet(Frames::RIGHT, health, Frames::RIGHT);
  stamina->PinSet(Frames::RIGHT, energy, Frames::RIGHT);

  Frames::Text *healthlabel = Frames::Text::Create(env->RootGet(), "HealthLabel");
  Frames::Text *energylabel = Frames::Text::Create(env->RootGet(), "HealthLabel");
  Frames::Text *staminalabel = Frames::Text::Create(env->RootGet(), "HealthLabel");

  healthlabel->TextSet("Health");
  energylabel->TextSet("Energy");
  staminalabel->TextSet("Stamina");

  healthlabel->ColorTextSet(tdc::white);
  energylabel->ColorTextSet(tdc::white);
  staminalabel->ColorTextSet(tdc::white);

  healthlabel->SizeSet(20);
  energylabel->SizeSet(20);
  staminalabel->SizeSet(20);

  healthlabel->PinSet(Frames::CENTERLEFT, health, Frames::CENTERRIGHT, 20, 0);
  energylabel->PinSet(Frames::CENTERLEFT, energy, Frames::CENTERRIGHT, 20, 0);
  staminalabel->PinSet(Frames::CENTERLEFT, stamina, Frames::CENTERRIGHT, 20, 0);

  AddDebugDisplay(env->RootGet(), stamina, Frames::TOPLEFT);
  AddDebugDisplay(env->RootGet(), energy, Frames::TOPLEFT);
  AddDebugDisplay(env->RootGet(), health, Frames::TOPLEFT);

  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_intro").Delta(2)); // arrow rotation

  energy->HeightSet(60);

  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_resize").Delta(2));  // arrow rotation

  // add health bar, numeric text
  const float healthCur = 4628;
  const float healthMax = 4820;
  
  const float energyCur = 100;
  const float energyMax = 100;
  
  const float staminaCur = 377;
  const float staminaMax = 1245;

  Frames::Text *healthtext = Frames::Text::Create(health, "HealthText");
  Frames::Text *energytext = Frames::Text::Create(energy, "EnergyText");
  Frames::Text *staminatext = Frames::Text::Create(stamina, "StaminaText");

  healthtext->TextSet(Frames::detail::Format("%d / %d", healthCur, healthMax));
  energytext->TextSet(Frames::detail::Format("%d / %d", energyCur, energyMax));
  staminatext->TextSet(Frames::detail::Format("%d / %d", staminaCur, staminaMax));

  healthtext->PinSet(Frames::BOTTOMRIGHT, health, Frames::BOTTOMRIGHT, -2, -2);
  energytext->PinSet(Frames::BOTTOMRIGHT, energy, Frames::BOTTOMRIGHT, -2, -2);
  staminatext->PinSet(Frames::BOTTOMRIGHT, stamina, Frames::BOTTOMRIGHT, -2, -2);

  healthtext->ColorTextSet(tdc::whiteBright);
  energytext->ColorTextSet(tdc::whiteBright);
  staminatext->ColorTextSet(tdc::whiteBright);

  AddDebugDisplay(env->RootGet(), healthtext, Frames::BOTTOMRIGHT, Frames::BOTTOMLEFT, 7, 7, true);
  AddDebugDisplay(env->RootGet(), energytext, Frames::BOTTOMRIGHT, Frames::BOTTOMLEFT, 7, 7, true);
  AddDebugDisplay(env->RootGet(), staminatext, Frames::BOTTOMRIGHT, Frames::BOTTOMLEFT, 7, 7, true);

  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_usage_corner").Delta(2));  // arrow rotation

  env->RootGet()->ChildGetByName("Arrows")->Obliterate();

  Frames::Frame *healthbar = Frames::Frame::Create(health, "HealthBar");
  Frames::Frame *energybar = Frames::Frame::Create(energy, "HealthBar");
  Frames::Frame *staminabar = Frames::Frame::Create(stamina, "HealthBar");
  
  // behind the text
  healthbar->LayerSet(-1);
  energybar->LayerSet(-1);
  staminabar->LayerSet(-1);

  healthbar->BackgroundSet(tdc::red);
  energybar->BackgroundSet(tdc::green);
  staminabar->BackgroundSet(tdc::blue);

  healthbar->PinSet(Frames::TOPLEFT, health, Frames::TOPLEFT);
  energybar->PinSet(Frames::TOPLEFT, energy, Frames::TOPLEFT);
  staminabar->PinSet(Frames::TOPLEFT, stamina, Frames::TOPLEFT);

  healthbar->PinSet(Frames::BOTTOMRIGHT, health, healthCur / healthMax, 1.f);
  energybar->PinSet(Frames::BOTTOMRIGHT, energy, energyCur / energyMax, 1.f);
  staminabar->PinSet(Frames::BOTTOMRIGHT, stamina, staminaCur / staminaMax, 1.f);

  Frames::Sprite *indicator = Frames::Sprite::Create(healthbar, "Indicator");
  indicator->TextureSet("indicator.png");
  indicator->PinSet(Frames::CENTER, healthbar, Frames::TOPRIGHT);

  healthtext->PinClear(Frames::BOTTOMRIGHT);
  energytext->PinClear(Frames::BOTTOMRIGHT);
  staminatext->PinClear(Frames::BOTTOMRIGHT);

  healthtext->PinSet(Frames::CENTER, health, Frames::CENTER);
  energytext->PinSet(Frames::CENTER, energy, Frames::CENTER);
  staminatext->PinSet(Frames::CENTER, stamina, Frames::CENTER);

  AddDebugDisplay(env->RootGet(), healthtext, Frames::CENTER, Frames::TOPLEFT, 7.f, 7.f);
  AddDebugDisplay(env->RootGet(), energytext, Frames::CENTER, Frames::TOPLEFT, 7.f, 7.f);
  AddDebugDisplay(env->RootGet(), staminatext, Frames::CENTER, Frames::TOPLEFT, 7.f, 7.f);

  AddDebugDisplay(env->RootGet(), indicator, Frames::CENTER, Frames::BOTTOMLEFT, 7, 7, true);

  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_usage_center"));

  env->RootGet()->ChildGetByName("Arrows")->Obliterate();

  Frames::Text *status = Frames::Text::Create(env->RootGet(), "StatusText");

  status->ColorTextSet(tdc::white);
  status->TextSet("Poisoned!");
  status->SizeSet(20);

  status->PinSet(Frames::TOPCENTER, stamina, Frames::BOTTOMCENTER, 0, 20);

  AddDebugDisplay(env->RootGet(), healthlabel, Frames::LEFTCENTER, Frames::CENTERLEFT, -5, -25, true);
  AddDebugDisplay(env->RootGet(), energylabel, Frames::LEFTCENTER, Frames::CENTERLEFT, -5, -25, true);
  AddDebugDisplay(env->RootGet(), staminalabel, Frames::LEFTCENTER, Frames::CENTERLEFT, -5, -25, true);

  AddDebugDisplay(env->RootGet(), status, Frames::CENTERTOP, Frames::TOPLEFT, 7, 0, true);

  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_usage_edge"));
}

TEST(Pinningbasics, Unidirectional) {
  TestEnvironment env(true, 640, 360);

  for (int i = 0; i < 3; ++i) {
    float ofs = i * 100.f;

    Frames::Frame *left = Frames::Frame::Create(env->RootGet(), "Red");
    Frames::Frame *right = Frames::Frame::Create(env->RootGet(), "Green");

    left->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 40, 40 + ofs);
    right->PinSet(Frames::TOPRIGHT, env->RootGet(), Frames::TOPRIGHT, -40, 40 + ofs);

    left->BackgroundSet(tdc::red);
    right->BackgroundSet(tdc::green);

    if (i == 0) {
      Frames::Text *descr = Frames::Text::Create(env->RootGet(), "Descr");
      descr->ColorTextSet(Frames::Color(1.f, 1.f, 0.f));
      descr->FontSet("geo_1.ttf");
      descr->TextSet("Neither frame pinned to the other");
      descr->PinSet(Frames::TOP, left, Frames::BOTTOM, Frames::Nil, 5);
      descr->PinSet(Frames::CENTERX, env->RootGet(), Frames::CENTERX);
    } else if (i == 1) {
      left->PinSet(Frames::BOTTOMRIGHT, right, Frames::BOTTOMLEFT, -80, 0);
      AddDebugDisplay(env->RootGet(), left, Frames::BOTTOMRIGHT, Frames::TOPRIGHT, 0, 5);
    } else if (i == 2) {
      right->PinSet(Frames::BOTTOMLEFT, left, Frames::BOTTOMRIGHT, 80, 0);
      AddDebugDisplay(env->RootGet(), right, Frames::BOTTOMLEFT, Frames::TOPLEFT, 0, 5);
    }
  }

  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_unidirectional"));
}

void ShowPosition(Frames::Layout *root, Frames::Frame *target, Frames::Anchor anchor, Frames::Anchor tanchor) {
  Frames::Frame *overlay = root->ChildGetByName("Overlay");
  if (!overlay) {
    overlay = Frames::Frame::Create(root, "Overlay");
  }

  Frames::Sprite *reticle = Frames::Sprite::Create(overlay, "Reticle");
  Frames::Text *text = Frames::Text::Create(overlay, "Text");

  reticle->TextureSet("sqtarget.png");

  Frames::Vector inverted = Frames::PointFromAnchor(tanchor);
  inverted.x = 1 - inverted.x;
  inverted.y = 1 - inverted.y;

  reticle->PinSet(Frames::CENTER, target, anchor);
  text->PinSet(inverted, reticle, tanchor, (inverted.x * 2 - 1) * -5, (inverted.y * 2 - 1) * -5);

  text->TextSet(Frames::DescriptorFromPoint(anchor));
  text->ColorTextSet(Frames::Color(1.f, 1.f, 0.f));
  text->FontSet("geo_1.ttf");
}

TEST(Pinningbasics, Position) {
  TestEnvironment env(true, 640, 360);

  CreateNameBar(env->RootGet(), "Named anchors provided by Frames");

  Frames::Frame *frame = Frames::Frame::Create(env->RootGet(), "Frame");
  frame->BackgroundSet(tdc::red);

  frame->WidthSet(160);
  frame->HeightSet(160);

  frame->PinSet(Frames::CENTER, env->RootGet(), Frames::CENTER, 0, 30);

  ShowPosition(env->RootGet(), frame, Frames::TOPLEFT, Frames::TOPLEFT);
  ShowPosition(env->RootGet(), frame, Frames::TOPRIGHT, Frames::TOPRIGHT);
  ShowPosition(env->RootGet(), frame, Frames::BOTTOMLEFT, Frames::BOTTOMLEFT);
  ShowPosition(env->RootGet(), frame, Frames::BOTTOMRIGHT, Frames::BOTTOMRIGHT);

  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_anchor_corner"));

  env->RootGet()->ChildGetByName("Overlay")->Obliterate();

  ShowPosition(env->RootGet(), frame, Frames::CENTER, Frames::BOTTOMCENTER);
  
  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_anchor_center"));

  env->RootGet()->ChildGetByName("Overlay")->Obliterate();

  ShowPosition(env->RootGet(), frame, Frames::TOPCENTER, Frames::TOPCENTER);
  ShowPosition(env->RootGet(), frame, Frames::BOTTOMCENTER, Frames::BOTTOMCENTER);
  ShowPosition(env->RootGet(), frame, Frames::CENTERLEFT, Frames::CENTERLEFT);
  ShowPosition(env->RootGet(), frame, Frames::CENTERRIGHT, Frames::CENTERRIGHT);

  TestSnapshot(env, SnapshotConfig().File("ref/doc/pinningbasics_anchor_edge"));
}
