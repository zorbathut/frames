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

#include "frames/const.h"

#include "frames/configuration.h"
#include "frames/detail.h"
#include "frames/detail_format.h"

namespace Frames {
  static const float Nil = detail::Reinterpret<float>(0xFFF00DFF);

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
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve descriptor from out-of-bounds anchor");
      return "";
    }
    return textRep[anchor];
  }

  /// Retrieves coordinates for an anchor point
  Vector PointFromAnchor(Anchor anchor) {
    if (anchor < 0 || anchor >= ANCHOR_COUNT) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve point from out-of-bounds anchor");
      return Vector(0.f, 0.f);
    }

    return Vector(detail::AnchorLookup[anchor].x, detail::AnchorLookup[anchor].y);
  }
}
