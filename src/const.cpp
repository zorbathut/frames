
#include "frames/const.h"

#include "frames/detail.h"
#include "frames/detail_format.h"

namespace Frames {
  const float Nil = detail::Reinterpret<float>(0xFFF00DFF);

  std::string DescriptorFromPoint(float x, float y) {
    for (int i = 0; i < ANCHOR_COUNT; ++i) {
      if ((x == detail::AnchorLookup[i].x || detail::IsNil(x) && detail::IsNil(detail::AnchorLookup[i].x)) &&
        (y == detail::AnchorLookup[i].y || detail::IsNil(y) && detail::IsNil(detail::AnchorLookup[i].y))) {
        return DescriptorFromPoint(Anchor(i));
      }
    }

    return detail::Format("%s", Vector(x, y));
  }
  
  static const char* textRep[] = {
    "TOPLEFT",
    "TOPCENTER",
    "TOPRIGHT",
    "CENTERLEFT",
    "CENTER",
    "CENTERRIGHT",
    "BOTTOMLEFT",
    "BOTTOMCENTER",
    "BOTTOMRIGHT",
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

  /// Retrieves coordinates for an anchor point
  Vector PointFromAnchor(Anchor anchor) {
    if (anchor < 0 || anchor >= ANCHOR_COUNT) {
      // TODO: barf
      return Vector(0.f, 0.f);
    }

    return Vector(detail::AnchorLookup[anchor].x, detail::AnchorLookup[anchor].y);
  }
}
