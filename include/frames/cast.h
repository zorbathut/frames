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

#ifndef FRAMES_CAST
#define FRAMES_CAST

#include "layout.h"
#include "noncopyable.h"

namespace Frames {
  namespace detail {
    class Rtti : Noncopyable {
    public:
      Rtti(const Rtti *parent) : m_parent(parent) { }

      const Rtti *ParentGet() const { return m_parent; }

    private:
      const Rtti *m_parent;
    };

    // This exists solely so we don't have to "public" RttiStaticGet()
    template <typename T> const Rtti *InitHelper() {
      return T::RttiStaticGet();
    }
  }

  /// Safe dynamic cast for the Layout hierarchy.
  /** Returns a valid pointer to T* if the provided Layout is actually a T*, or null otherwise. */
  template <typename T> T *Cast(Layout *layout) {
    return const_cast<T *>(Cast<T>(const_cast<const Layout *>(layout))); // yes I know this is nasty
  }

  /// Safe dynamic cast for the Layout hierarchy.
  /** Returns a valid pointer to T* if the provided Layout is actually a T*, or null otherwise. */
  template <typename T> const T *Cast(const Layout *layout) {
    const detail::Rtti *target = T::RttiStaticGet();
    const detail::Rtti *current = layout->RttiVirtualGet();
    while (current) {
      if (current == target) {
        return static_cast<const T*>(layout);
      }
      current = current->ParentGet();
    }
    return 0; // nope
  }

  #define FRAMES_DEFINE_RTTI(derived, base) \
    ::Frames::detail::Rtti derived::s_rtti(::Frames::detail::InitHelper<base>()); \
    /*static*/ const char *derived::TypeStaticGet() { \
      return #derived; \
    }
}

#endif
