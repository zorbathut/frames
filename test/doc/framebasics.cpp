
#include <gtest/gtest.h>

#include <frames/frame.h>

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
    frame->BackgroundSet(tdc::bluegreen);
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

  TestSnapshot(env, "ref/doc/framebasics_example");
};
