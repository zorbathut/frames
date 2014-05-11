// Constant values and associated debug output

#ifndef FRAMES_CONST
#define FRAMES_CONST

#include <string>

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

  /// Converts a point into a human-readable string.
  std::string DescriptorFromPoint(float x, float y);
  /// Converts a point into a human-readable string.
  std::string DescriptorFromPoint(Anchor anchor);
}

#endif
