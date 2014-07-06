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

#ifndef FRAMES_COLOR
#define FRAMES_COLOR

namespace Frames {
  /// Represents an RGBA color value.
  /** Each value is on a 0-to-1 scale, where 1 is fully lit or fully opaque. */
  class Color {
  public:
    Color() : r(0), g(0), b(0), a(1) { };
    Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) { };
    union {
      struct {
        /// Red color channel.
        float r;
        /// Green color channel.
        float g;
        /// Blue color channel.
        float b;
        /// Alpha color channel. 0 is transparent, 1 is opaque.
        float a;
      };
      /// Array representation, in RGBA order. Can be used interchangably with the other members.
      float c[4];
    };
  };

  inline bool operator==(const Color &lhs, const Color &rhs) {
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
  }

  inline bool operator!=(const Color &lhs, const Color &rhs) {
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
  }

  inline Color operator*(const Color &lhs, const Color &rhs) {
    return Color(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a);
  }
}

#endif
