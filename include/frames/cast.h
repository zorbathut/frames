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
  /** Returns a valid pointer to T* if the provided Layout is actually a T*, or a null pointer otherwise. */
  template <typename T> T *Cast(Layout *layout) {
    return const_cast<T *>(Cast<T>(const_cast<const Layout *>(layout))); // yes I know this is nasty
  }

  /// Safe dynamic cast for the Layout hierarchy.
  /** Returns a valid pointer to T* if the provided Layout is actually a T*, or a null pointer otherwise. */
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
    detail::Rtti derived::s_rtti(detail::InitHelper<base>());
}

#endif
