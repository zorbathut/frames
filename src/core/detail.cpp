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

#include "frames/detail.h"

#include "frames/frame.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <cstdio>
#include <cstdarg>

// for the undefined types
BOOST_STATIC_ASSERT(sizeof(float) == sizeof(int));

namespace Frames {
  namespace detail {
    const Vector AnchorLookup[ANCHOR_COUNT] = {
      Vector(0.f, 0.f),  // TOPLEFT
      Vector(0.5f, 0.f),  // TOPCENTER
      Vector(1.f, 0.f),  // TOPRIGHT
      Vector(0.f, 0.5f),  // CENTERLEFT
      Vector(0.5f, 0.5f),  // CENTER
      Vector(1.f, 0.5f),  // CENTERRIGHT
      Vector(0.f, 1.f),  // BOTTOMLEFT
      Vector(0.5f, 1.f),  // BOTTOMCENTER
      Vector(1.f, 1.f),  // BOTTOMRIGHT

      // These constants are supposed to be Nil, but they're hardcoded to fix constant initialization order issues on some compilers. Can be fixed by putting them in a single file, in theory.
      Vector(0.f, detail::Reinterpret<float>(0xFFF00DFF)),  // LEFT
      Vector(0.5f, detail::Reinterpret<float>(0xFFF00DFF)),  // CENTERX
      Vector(1.f, detail::Reinterpret<float>(0xFFF00DFF)),  // RIGHT
      Vector(detail::Reinterpret<float>(0xFFF00DFF), 0.f),  // TOP
      Vector(detail::Reinterpret<float>(0xFFF00DFF), 0.5f),  // CENTERY
      Vector(detail::Reinterpret<float>(0xFFF00DFF), 1.f),  // BOTTOM
    };

    int ClampToPowerOf2(int input) {
      input--;
      input = (input >> 1) | input;
      input = (input >> 2) | input;
      input = (input >> 4) | input;
      input = (input >> 8) | input;
      input = (input >> 16) | input;
      return input + 1;
    }

    bool FrameOrderSorter::operator()(const Frame *lhs, const Frame *rhs) const {
      if (lhs->zinternalImplementationGet() != rhs->zinternalImplementationGet())
        return lhs->zinternalImplementationGet() < rhs->zinternalImplementationGet();
      if (lhs->zinternalLayerGet() != rhs->zinternalLayerGet())
        return lhs->zinternalLayerGet() < rhs->zinternalLayerGet();
      // they're the same, but we want a consistent sort that won't result in Z-fighting
      return lhs->m_constructionOrder < rhs->m_constructionOrder;
    }

    bool LayoutIdSorter::operator()(const Layout *lhs, const Layout *rhs) const {
      // this is just for the sake of a consistent sort across executions
      return lhs->m_constructionOrder < rhs->m_constructionOrder;
    }

    bool operator<(const TextFormat &lhs, const TextFormat &rhs) {
      if (lhs.size != rhs.size)
        return lhs.size < rhs.size;
      if (lhs.fill != rhs.fill)
        return lhs.fill < rhs.fill;
      if (lhs.stroke != rhs.stroke)
        return lhs.stroke < rhs.stroke;
      if (lhs.strokeRadius != rhs.strokeRadius)
        return lhs.strokeRadius < rhs.strokeRadius;
      if (lhs.strokeCap != rhs.strokeCap)
        return lhs.strokeCap < rhs.strokeCap;
      if (lhs.strokeJoin != rhs.strokeJoin)
        return lhs.strokeJoin < rhs.strokeJoin;
      if (lhs.strokeMiterLimit != rhs.strokeMiterLimit)
        return lhs.strokeMiterLimit < rhs.strokeMiterLimit;
      return false;
    }
  }
}

