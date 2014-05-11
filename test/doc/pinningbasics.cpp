
#include <gtest/gtest.h>

#include <frames/cast.h>
#include <frames/frame.h>
#include <frames/text.h>
#include <frames/raw.h>
#include <frames/detail_format.h>

#include "lib.h"
#include "doc/colors.h"

class Arrow : public Frames::Raw {
  FRAMES_DECLARE_RTTI();
public:
  static Arrow *Create(Frames::Layout *parent, const std::string &name) {
    return new Arrow(parent, name);
  }

private:
  Arrow(Frames::Layout *parent, const std::string &name) : Raw(parent, name) {
    EventAttach(Frames::Raw::Event::Render, Frames::Delegate<void(Frames::Handle*)>(this, &Arrow::Render));
  }

  void Render(Frames::Handle *) {
    Frames::Rect bounds = BoundsGet();

    float ang = atan2(bounds.e.x - bounds.s.x, bounds.e.y - bounds.s.y);
    const float linelen = 8;
    const float lineang = 0.6f;

    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(bounds.s.x, bounds.s.y);
    glVertex2f(bounds.e.x, bounds.e.y);
    glVertex2f(bounds.e.x, bounds.e.y);
    glVertex2f(bounds.e.x - sin(ang + lineang) * linelen, bounds.e.y - cos(ang + lineang) * linelen);
    glVertex2f(bounds.e.x, bounds.e.y);
    glVertex2f(bounds.e.x - sin(ang - lineang) * linelen, bounds.e.y - cos(ang - lineang) * linelen);
    glEnd();
  }
};
FRAMES_DEFINE_RTTI(Arrow, Frames::Raw);

void AddDebugDisplay(Frames::Layout *root, Frames::Layout *target, Frames::Anchor point, Frames::Anchor texanchor = Frames::CENTERLEFT, float tdx = 5, float tdy = 0) {
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
  tex->TextSet(Frames::detail::Format("Pin from %s.%s to %s.%s with an offset of %s", target->NameGet(), Frames::DescriptorFromPoint(point), ppt.target->NameGet(), Frames::DescriptorFromPoint(ppt.point.x, ppt.point.y), ppt.offset));
  tex->PinSet(texanchor, arrow, Frames::CENTER, tdx, tdy);
  tex->ColorTextSet(Frames::Color(1.f, 1.f, 0.f));
  tex->FontSet("geo_1.ttf");
}

// Disabled because the lines aren't pixel-perfect. TODO fix
TEST(Pinningbasics, DISABLED_Example) {
  TestEnvironment env(true, 640, 360);

  Frames::Frame *health = Frames::Frame::Create(env->RootGet(), "Health");
  Frames::Frame *energy = Frames::Frame::Create(env->RootGet(), "Energy");
  Frames::Frame *stamina = Frames::Frame::Create(env->RootGet(), "Stamina");

  health->HeightSet(30);
  energy->HeightSet(30);
  stamina->HeightSet(30);

  health->BackgroundSet(tdc::red);
  energy->BackgroundSet(tdc::green);
  stamina->BackgroundSet(tdc::blue);

  health->PinSet(Frames::TOPLEFT, env->RootGet(), Frames::TOPLEFT, 40, 40);
  energy->PinSet(Frames::TOPLEFT, health, Frames::BOTTOMLEFT, 0, 20);
  stamina->PinSet(Frames::TOPLEFT, energy, Frames::BOTTOMLEFT, 0, 20);

  health->WidthSet(200);
  energy->PinSet(Frames::RIGHT, health, Frames::RIGHT);
  stamina->PinSet(Frames::RIGHT, energy, Frames::RIGHT);

  Frames::Text *healthtext = Frames::Text::Create(env->RootGet(), "HealthText");
  Frames::Text *energytext = Frames::Text::Create(env->RootGet(), "HealthText");
  Frames::Text *staminatext = Frames::Text::Create(env->RootGet(), "HealthText");

  healthtext->TextSet("Health");
  energytext->TextSet("Energy");
  staminatext->TextSet("Stamina");

  healthtext->ColorTextSet(tdc::white);
  energytext->ColorTextSet(tdc::white);
  staminatext->ColorTextSet(tdc::white);

  healthtext->SizeSet(20);
  energytext->SizeSet(20);
  staminatext->SizeSet(20);

  healthtext->PinSet(Frames::CENTERLEFT, health, Frames::CENTERRIGHT, 20, 0);
  energytext->PinSet(Frames::CENTERLEFT, energy, Frames::CENTERRIGHT, 20, 0);
  staminatext->PinSet(Frames::CENTERLEFT, stamina, Frames::CENTERRIGHT, 20, 0);

  AddDebugDisplay(env->RootGet(), stamina, Frames::TOPLEFT);
  AddDebugDisplay(env->RootGet(), energy, Frames::TOPLEFT);
  AddDebugDisplay(env->RootGet(), health, Frames::TOPLEFT);

  TestSnapshot(env, "ref/doc/pinningbasics_intro");

  energy->HeightSet(60);

  TestSnapshot(env, "ref/doc/pinningbasics_resize");
}

// Disabled because the lines aren't pixel-perfect. TODO fix
TEST(Pinningbasics, DISABLED_Unidirectional) {
  TestEnvironment env(true, 640, 360);

  for (int i = 0; i < 3; ++i) {
    float ofs = i * 100.f;

    Frames::Frame *left = Frames::Frame::Create(env->RootGet(), "Left");
    Frames::Frame *right = Frames::Frame::Create(env->RootGet(), "Right");

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

  TestSnapshot(env, "ref/doc/pinningbasics_unidirectional");
}
