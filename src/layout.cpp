
#include "frames/layout.h"
#include "frames/environment.h"

namespace Frames {
  float Layout::GetPoint(Axis axis, float pt) const {
    const AxisData &ax = m_axes[axis];

    // Check our caches
    const AxisData::Connector &axa = ax.connections[0];
    if (axa.point_mine == pt) {
      if (!Utility::IsUndefined(axa.cached)) {
        return axa.cached;
      }
      if (axa.link) {
        axa.cached = axa.link->GetPoint(axis, axa.point_link) + axa.offset;
        return axa.cached;
      }
      axa.cached = axa.offset;
      return axa.cached;
    }

    const AxisData::Connector &axb = ax.connections[1];
    if (axb.point_mine == pt) {
      if (!Utility::IsUndefined(axb.cached)) {
        return axb.cached;
      }
      if (axb.link) {
        axb.cached = axb.link->GetPoint(axis, axb.point_link) + axb.offset;
        return axb.cached;
      }
      axb.cached = axb.offset;
      return axb.cached;
    }

    // Easy work done, let's do the hard stuff
    // Possibilities:
    // 0 vertices, no size: Defer to default size and "0 vertices, size" case
    // 0 vertices, size: Assume top edge is pinned to 0
    // 1 vertex, no size: Defer to default size and "1 vertex, size" case
    // 1 vertex, size: Place as appropriate
    // 2 vertices, no size: Calculate size (we'll need it anyway) and defer to "1 vertex, size"
    // In other words, the only cases we actually care about are "0 vertices, size" and "1 vertex, size"
    // In all cases, we assume size exists via GetSize() - the only question is whether we have a vertex or not

    // Find a valid vertex - we'll only be using one
    const AxisData::Connector *connect;
    if (Utility::IsUndefined(axa.point_mine)) {
      connect = &axb;
    } else {
      connect = &axa;
    }

    // 0 vertices, size
    if (Utility::IsUndefined(connect->point_mine)) {
      return pt * GetSize(axis);
    }

    // 1 vertex, size
    return GetPoint(axis, connect->point_mine) + (pt - connect->point_mine) * GetSize(axis);
  }

  float Layout::GetSize(Axis axis) const {
    const AxisData &ax = m_axes[axis];

    // Check our cache
    if (!Utility::IsUndefined(ax.size_cached)) {
      return ax.size_cached;
    }

    // Check an explicit setting
    if (!Utility::IsUndefined(ax.size_set)) {
      ax.size_cached = ax.size_set;
      return ax.size_set;
    }

    // Let's see if we have two known points
    const AxisData::Connector &axa = ax.connections[0];
    const AxisData::Connector &axb = ax.connections[1];
    if (!Utility::IsUndefined(axa.point_mine) && !Utility::IsUndefined(axb.point_mine)) {
      // Woo! We can calculate from here
      float a = GetPoint(axis, axa.point_mine);
      float b = GetPoint(axis, axb.point_mine);

      ax.size_cached = (a - b) / (axa.point_mine - axb.point_mine);
      return ax.size_cached;
    }

    // Default size it is
    ax.size_cached = ax.size_default;
    return ax.size_default;
  }

  Layout::Layout(const LayoutPtr &layout, Environment *env) :
      m_resolved(false),
      m_layer(0),
      m_strata(0),
      m_visible(true)
  {
    if (layout && env) {
      assert(layout->GetEnvironment() == env);
    }
    assert(layout || env);

    if (layout) {
      m_env = layout->GetEnvironment();
    } else {
      m_env = env;
    }

    m_env->MarkInvalidated(this); // We'll need to resolve this before we go

    if (layout) {
      SetParent(layout);
    }
  }

  Layout::~Layout() {
    // Right now, we're just not doing anything - things will be cleaned up as appropriate. Later, we'll want to clean up everything not marked for delete, and invalidate/scream bloody murder if that occurs
  }

  void Layout::SetPoint(Axis axis, float mypt, const LayoutPtr &link, float linkpt, float offset) {
    AxisData &ax = m_axes[axis];

    AxisData::Connector &axa = ax.connections[0];
    if (axa.point_mine == mypt) {
      // We don't care if we haven't changed
      if (axa.link == link && (!link || axa.point_link != linkpt) && axa.offset != offset) {
        return;
      }

      // If we've been used, then we need to invalidate
      if (!Utility::IsUndefined(axa.cached)) {
        Invalidate(axis);
      }

      if (axa.link != link) {
        if (axa.link) {
          axa.link->m_axes[axis].children.erase(this);
        }
        if (link) {
          link->m_axes[axis].children.insert(this);
        }
      }

      axa.link = link;
      axa.point_link = linkpt;
      axa.offset = offset;
      
      return;
    }

    AxisData::Connector &axb = ax.connections[1];
    if (axb.point_mine == mypt) {
      // We don't care if we haven't changed
      if (axb.link == link && (!link || axb.point_link != linkpt) && axb.offset != offset) {
        return;
      }

      // If we've been used, then we need to invalidate
      if (!Utility::IsUndefined(axb.cached)) {
        Invalidate(axis);
      }

      if (axb.link != link) {
        if (axb.link) {
          axb.link->m_axes[axis].children.erase(this);
        }
        if (link) {
          link->m_axes[axis].children.insert(this);
        }
      }

      axb.link = link;
      axb.point_link = linkpt;
      axb.offset = offset;
      
      return;
    }

    bool axau = Utility::IsUndefined(axa.point_mine);
    bool axbu = Utility::IsUndefined(axb.point_mine);

    if (!Utility::IsUndefined(ax.size_set) && (!axau || !axbu)) {
      // TODO: error, too many things set
      return;
    }

    if (!axau && !axbu) {
      // TODO: error, too many things set
      return;
    }

    AxisData::Connector *replace;
    if (axau) {
      replace = &axa;
    } else {
      replace = &axb;
    }

    Invalidate(axis); // Adding a new point is always dangerous

    replace->point_mine = mypt;
    replace->link = link;
    replace->point_link = linkpt;
    replace->offset = offset;

    return;
  }

  void Layout::ClearPoint(Axis axis, float mypt) {
    AxisData &ax = m_axes[axis];

    AxisData::Connector &axa = ax.connections[0];
    if (axa.point_mine == mypt) {
      if (!Utility::IsUndefined(axa.cached)) {
        Invalidate(axis);
      }

      if (axa.link) {
        axa.link->m_axes[axis].children.erase(this);
      }

      axa.link = 0;
      axa.point_mine = Utility::Undefined;
      axa.point_link = Utility::Undefined;
      axa.offset = Utility::Undefined;

      return;
    }

    AxisData::Connector &axb = ax.connections[1];
    if (axb.point_mine == mypt) {
      if (!Utility::IsUndefined(axb.cached)) {
        Invalidate(axis);
      }

      if (axb.link) {
        axb.link->m_axes[axis].children.erase(this);
      }

      axb.link = 0;
      axb.point_mine = Utility::Undefined;
      axb.point_link = Utility::Undefined;
      axb.offset = Utility::Undefined;

      return;
    }

    // If we didn't actually clear anything, no sweat, no need to invalidate
  }

  void Layout::ClearAllPoints(Axis axis) {
    AxisData &ax = m_axes[axis];

    AxisData::Connector &axa = ax.connections[0];
    if (!Utility::IsUndefined(axa.point_mine)) {
      ClearPoint(axis, axa.point_mine);
    }

    AxisData::Connector &axb = ax.connections[1];
    if (!Utility::IsUndefined(axb.point_mine)) {
      ClearPoint(axis, axb.point_mine);
    }
  }

  void Layout::SetSize(Axis axis, float size) {
    AxisData &ax = m_axes[axis];

    // We don't care if we haven't changed
    if (ax.size_set != size) {
      if (!Utility::IsUndefined(ax.size_cached)) {
        Invalidate(axis);
      }

      ax.size_set = size;
    }
  }

  void Layout::ClearSize(Axis axis) {
    AxisData &ax = m_axes[axis];

    // We don't care if we haven't changed
    if (!Utility::IsUndefined(ax.size_set)) {
      if (!Utility::IsUndefined(ax.size_cached)) {
        Invalidate(axis);
      }

      ax.size_set = Utility::Undefined;
    }
  }

  void Layout::SetSizeDefault(Axis axis, float size) {
    AxisData &ax = m_axes[axis];

    // We don't care if we haven't changed
    if (ax.size_default != size) {
      // Invalidate if this size was actually important and used
      if (ax.size_cached == ax.size_default) {
        Invalidate(axis);
      }

      ax.size_default = size;
    }
  }

  void Layout::SetParent(const LayoutPtr &layout) {
    if (m_parent == layout) {
      return;
    }

    if (m_env != layout->GetEnvironment()) {
      assert(false);
      return;
    }

    // First, remove ourselves from our old parent
    if (m_parent) {
      m_parent->m_children.erase(this);
    }

    m_parent = layout;

    if (m_parent) {
      m_parent->m_children.insert(this);
    }
  }

  void Layout::SetLayer(float layer) {
    if (m_layer == layer) {
      return;
    }

    // We take ourselves out of our parents' list so we can change our layer without fucking up the sort
    if (m_parent) {
      m_parent->m_children.erase(this);
    }

    m_layer = layer;

    if (m_parent) {
      m_parent->m_children.insert(this);
    }
  }

  void Layout::SetStrata(float strata) {
    if (m_strata == strata) {
      return;
    }

    // We take ourselves out of our parents' list so we can change our layer without fucking up the sort
    if (m_parent) {
      m_parent->m_children.erase(this);
    }

    m_strata = m_strata;

    if (m_parent) {
      m_parent->m_children.insert(this);
    }
  }

  void Layout::SetVisible(bool visible) {
    if (m_visible == visible) {
      return;
    }

    m_visible = visible;
  }

  void Layout::Render() {
    if (m_visible) {
      // TODO: render background

      RenderElement();

      for (ChildrenList::const_iterator itr = m_children.begin(); itr != m_children.end(); ++itr) {
        (*itr)->Render();
      }
    }
  }

  // Technically, invalidate could be a lot more specific. There are cases where invalidating an entire axis is unnecessary - A depends on one connection, B depends on another connection, and only A's connection is invalidated.
  // It's unclear if this would be worth the additional overhead and complexity.
  // Invalidating extra stuff, while slow, is at least always correct.
  void Layout::Invalidate(Axis axis) const {
    // see if anything needs invalidating
    const AxisData &ax = m_axes[axis];

    if (!Utility::IsUndefined(ax.size_cached) || !Utility::IsUndefined(ax.connections[0].cached) || !Utility::IsUndefined(ax.connections[1].cached)) {
      // Do these first so we don't get ourselves trapped in an infinite loop
      ax.size_cached = Utility::Undefined;
      ax.connections[0].cached = Utility::Undefined;
      ax.connections[1].cached = Utility::Undefined;

      for (ChildrenList::const_iterator itr = ax.children.begin(); itr != ax.children.end(); ++itr) {
        (*itr)->Invalidate(axis);
      }

      if (m_resolved) {
        m_resolved = false;
        m_env->MarkInvalidated(const_cast<Layout*>(this));  // strip the constness, mostly because I'm not interested in making a ConstLayoutPtr
      }
    }
  }

  void Layout::Resolve() const {
    GetLeft();
    GetRight();
    GetTop();
    GetBottom();
    
    GetWidth();
    GetHeight();

    m_resolved = true;

    // Todo: queue up movement events
  }

  bool Layout::Sorter::operator()(const LayoutPtr &lhs, const LayoutPtr &rhs) const {
    if (lhs->GetStrata() != rhs->GetStrata())
      return lhs->GetStrata() < rhs->GetStrata();
    if (lhs->GetLayer() != rhs->GetLayer())
      return lhs->GetLayer() < rhs->GetLayer();
    // they're the same, but we want a consistent sort that won't result in Z-fighting
    return lhs.get() < rhs.get();
  }

  void intrusive_ptr_add_ref(Frames::Layout *layout) { }
  void intrusive_ptr_release(Frames::Layout *layout) { }
}

