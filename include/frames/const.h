// Constant values and associated debug output

#ifndef FRAMES_CONST
#define FRAMES_CONST

#include <string>

namespace Frames {
  struct Vector;

  enum Axis { X, Y }; // axes
  enum Anchor {
    TOPLEFT,
    LEFTTOP = TOPLEFT,
    TOPCENTER,
    CENTERTOP = TOPCENTER,
    TOPRIGHT,
    RIGHTTOP = TOPRIGHT,
    CENTERLEFT,
    LEFTCENTER = CENTERLEFT,
    CENTER,
    CENTERCENTER = CENTER,
    CENTERRIGHT,
    RIGHTCENTER = CENTERRIGHT,
    BOTTOMLEFT,
    LEFTBOTTOM = BOTTOMLEFT,
    BOTTOMCENTER,
    CENTERBOTTOM = BOTTOMCENTER,
    BOTTOMRIGHT,
    RIGHTBOTTOM = BOTTOMRIGHT,
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

  /// Retrieves coordinates for an anchor point
  Vector PointFromAnchor(Anchor anchor);
}

#endif
