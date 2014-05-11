
#include "frames/const.h"

#include "frames/detail.h"
#include "frames/detail_format.h"

namespace Frames {
  const float Nil = detail::Reinterpret<float>(0xFFF00DFF);

  std::string DescriptorFromPoint(float x, float y) {
    for (int i = 0; i < ANCHOR_COUNT; ++i) {
      if ((x == detail::c_anchorLookup[i].x || detail::IsNil(x) && detail::IsNil(detail::c_anchorLookup[i].x)) &&
        (y == detail::c_anchorLookup[i].y || detail::IsNil(y) && detail::IsNil(detail::c_anchorLookup[i].y))) {
        return DescriptorFromPoint(Anchor(i));
      }
    }

    return detail::Format("%s", Vector(x, y));
  }
  
  static const char* textRep[] = {
    "TOPLEFT",
    "LEFTTOP",
    "TOPCENTER",
    "CENTERTOP",
    "TOPRIGHT",
    "RIGHTTOP",
    "CENTERLEFT",
    "LEFTCENTER",
    "CENTER",
    "CENTERCENTER",
    "CENTERRIGHT",
    "RIGHTCENTER",
    "BOTTOMLEFT",
    "LEFTBOTTOM",
    "BOTTOMCENTER",
    "CENTERBOTTOM",
    "BOTTOMRIGHT",
    "RIGHTBOTTOM",
    "LEFT",
    "CENTERX",
    "RIGHT",
    "TOP",
    "CENTERY",
    "BOTTOM",
  };

  std::string DescriptorFromPoint(Anchor anchor) {
    if (anchor < 0 || anchor >= ANCHOR_COUNT) {
      // TODO: barf
      return "";
    }
    return textRep[anchor];
  }
}
