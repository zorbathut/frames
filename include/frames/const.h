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

  enum FontStrokeCap {
    FONTSTROKECAP_ROUND, ///< Round end caps at the end of lines.
    FONTSTROKECAP_SQUARE, ///< Square end caps at the end of lines.
    FONTSTROKECAP_BUTT, ///< Flatly truncated end caps at the end of lines.
  };
  enum FontStrokeJoin {
    FONTSTROKEJOIN_ROUND, ///< Round joints at line angles.
    FONTSTROKEJOIN_BEVEL, ///< Beveled joints at line angles.
    FONTSTROKEJOIN_MITER, ///< Mitered joints at line angles. Miter will be truncated according to the Miterlimit parameter.
  };
}

#endif
