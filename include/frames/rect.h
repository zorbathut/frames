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

#ifndef FRAMES_RECT
#define FRAMES_RECT

#include "frames/vector.h"

namespace Frames {
  /// Represents a rectangular area in space.
  /** This is nothing more than a pair of Vectors tied together. As is usual with Rect classes, the actual meaning of this structure depends strongly on its context. */
  struct Rect {
    /// Creates an uninitialized Rect with undefined values.
    Rect() { }
    /// Creates a Rect from a start point and an end point.
    Rect(const Vector &s, const Vector &e) : s(s), e(e) { }
    /// Creates a Rect from four coordinates; start-X, start-Y, end-X, and end-Y.
    Rect(float sx, float sy, float ex, float ey) : s(sx, sy), e(ex, ey) { }

    /// Represents the topleft corner, or "start", of the rectangle.
    Vector s;
    /// Represents the bottomright corner, or "end", of the rectangle.
    Vector e;
  };
}

#endif
