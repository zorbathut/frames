// Constant values

#ifndef FRAMES_CONST
#define FRAMES_CONST

namespace Frames {
  enum Axis { X, Y }; // axes
  enum Anchor {
    TOPLEFT,
    LEFTTOP,
    TOPCENTER,
    CENTERTOP,
    TOPRIGHT,
    RIGHTTOP,
    CENTERLEFT,
    LEFTCENTER,
    CENTER,
    CENTERCENTER,
    CENTERRIGHT,
    RIGHTCENTER,
    BOTTOMLEFT,
    LEFTBOTTOM,
    BOTTOMCENTER,
    CENTERBOTTOM,
    BOTTOMRIGHT,
    RIGHTBOTTOM,
    LEFT,
    CENTERX,
    RIGHT,
    TOP,
    CENTERY,
    BOTTOM,

    ANCHOR_COUNT,
  }; // axes

  extern const float Nil;
}

#endif
