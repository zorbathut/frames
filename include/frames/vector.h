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

#ifndef FRAMES_VECTOR
#define FRAMES_VECTOR

#include <ostream>

namespace Frames {
  /// Represents a 2d location or direction.
  /** This is nothing more than a pair of floats tied together. As is usual with Vector classes, the actual meaning of this structure depends strongly on its context. */
  struct Vector {
    /// Default constructor.
    /** Does not initialize to any defined value. */
    Vector() { };
    
    /// Initializes to provided coordinates.
    Vector(float x, float y) : x(x), y(y) { };

    /// X coordinate.
    float x;
    
    /// Y coordinate.
    float y;
  };
  std::ostream &operator<<(std::ostream &str, const Vector &rhs);
  
  inline Vector operator+(const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.x + rhs.x, lhs.y + rhs.y);
  }
  inline Vector operator-(const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.x - rhs.x, lhs.y - rhs.y);
  }

  inline Vector operator*(const Vector &lhs, float rhs) {
    return Vector(lhs.x * rhs, lhs.y * rhs);
  }
  inline Vector operator/(const Vector &lhs, float rhs) {
    return Vector(lhs.x / rhs, lhs.y / rhs);
  }

  inline void operator+=(Vector &lhs, const Vector &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
  }
  inline void operator-=(Vector &lhs, const Vector &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
  }

  inline void operator*=(Vector &lhs, float rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
  }
  inline void operator/=(Vector &lhs, float rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
  }

  inline bool operator==(const Vector &lhs, const Vector &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
  inline bool operator!=(const Vector &lhs, const Vector &rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y;
  }

  namespace detail {
    // prototyped so we can call it without including utility.h
    float Clamp(float current, float min, float max);

    inline Vector Clamp(const Vector &current, const Vector &min, const Vector &max) {
      return Vector(Clamp(current.x, min.x, max.x), Clamp(current.y, min.y, max.y));
    }
  }
}

#endif
