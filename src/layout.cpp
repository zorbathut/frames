
#include "frames/layout.h"

#include "frames/environment.h"
#include "frames/rect.h"
#include "frames/renderer.h"

#include "boost/static_assert.hpp"

#include <lua.hpp>

namespace Frames {
  /*static*/ const char *Layout::GetStaticType() {
    return "Layout";
  }

  float Layout::GetPoint(Axis axis, float pt) const {
    const AxisData &ax = m_axes[axis];

    // Check our caches
    const AxisData::Connector &axa = ax.connections[0];
    if (axa.point_mine == pt) {
      if (!Utility::IsUndefined(axa.cached)) {
        if (Utility::IsProcessing(axa.cached)) {
          m_env->LayoutStack_Push(this, axis, pt);
          m_env->LayoutStack_Error();
          m_env->LayoutStack_Pop();
          axa.cached = 0./0.; // generate a real NaN
        }
        return axa.cached;
      }
      if (axa.link) {
        m_env->LayoutStack_Push(this, axis, pt);
        axa.cached = Utility::Processing; // seed it with processing so we'll exit if this turns out to be an infinite loop
        axa.cached = axa.link->GetPoint(axis, axa.point_link) + axa.offset;
        m_env->LayoutStack_Pop();
        return axa.cached;
      }
      axa.cached = axa.offset;
      return axa.cached;
    }

    const AxisData::Connector &axb = ax.connections[1];
    if (axb.point_mine == pt) {
      if (!Utility::IsUndefined(axb.cached)) {
        if (Utility::IsProcessing(axa.cached)) {
          m_env->LayoutStack_Push(this, axis, pt);
          m_env->LayoutStack_Error();
          m_env->LayoutStack_Pop();
          axb.cached = 0./0.; // generate a real NaN
        }
        return axb.cached;
      }
      if (axb.link) {
        m_env->LayoutStack_Push(this, axis, pt);
        axb.cached = Utility::Processing; // seed it with processing so we'll exit if this turns out to be an infinite loop
        axb.cached = axb.link->GetPoint(axis, axb.point_link) + axb.offset;
        m_env->LayoutStack_Pop();
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
      m_env->LayoutStack_Push(this, axis, pt);
      float rv = pt * GetSize(axis);
      m_env->LayoutStack_Pop();
      return rv;
    }

    // 1 vertex, size
    m_env->LayoutStack_Push(this, axis, pt);
    float rv = GetPoint(axis, connect->point_mine) + (pt - connect->point_mine) * GetSize(axis);
    m_env->LayoutStack_Pop();
    return rv;
  }

  Rect Layout::GetBounds() const {
    return Rect(GetLeft(), GetTop(), GetRight(), GetBottom());
  }

  float Layout::GetSize(Axis axis) const {
    const AxisData &ax = m_axes[axis];

    // Check our cache
    if (!Utility::IsUndefined(ax.size_cached)) {
      if (Utility::IsProcessing(ax.size_cached)) {
        m_env->LayoutStack_Push(this, axis);
        m_env->LayoutStack_Error();
        m_env->LayoutStack_Pop();
        ax.size_cached = 0./0.; // generate a real NaN
      }
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
      m_env->LayoutStack_Push(this, axis);
      ax.size_cached = Utility::Processing; // seed it with processing so we'll exit if this turns out to be an infinite loop
      float a = GetPoint(axis, axa.point_mine);
      float b = GetPoint(axis, axb.point_mine);
      m_env->LayoutStack_Pop();

      ax.size_cached = (a - b) / (axa.point_mine - axb.point_mine);
      return ax.size_cached;
    }

    // Default size it is
    ax.size_cached = ax.size_default;
    return ax.size_default;
  }

  void Layout::l_push(lua_State *L) const {
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rrg");

    if (lua_isnil(L, -1)) {
      FRAMES_ERROR("Attempting to push frame into environment without Frames registered");
      return; // sure, it's nil, we'll just leave the nil in there
    }

    lua_pushlightuserdata(L, (void*)this);
    lua_rawget(L, -2);

    if (!lua_isnil(L, -1)) {
      // we found it! strip out the item we don't want to push
      lua_remove(L, -2);
      // and we're done!
      // nice fast execution path.
    } else {
      // we didn't find it, this is far more complex
      // we'll need to do a few things:
      // * generate an appropriate table, with attached metatable
      // * register that table in _rrg
      // * register that table in all appropriate _rg's
      
      // first off, we'll need to kill the nil we just pulled out
      lua_pop(L, 1);

      // generate the table we'll be using to represent the whole thing
      lua_newtable(L);

      // add the key/value pair
      lua_pushlightuserdata(L, (void*)this);
      lua_pushvalue(L, -2);

      // stack: ... Frames_rrg newtab luserdata newtab

      // push the pair into the lookup table
      lua_rawset(L, -4);

      // stack: ... Frames_rrg newtab

      // Now we can kill Frames_rrg
      lua_remove(L, -2);

      // stack: ... newtab

      // Next, let's set the metatable to the one that's stored
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_mt");
      lua_getfield(L, -1, GetType());

      if (lua_isnil(L, -1)) {
        // fffff
        FRAMES_ERROR("Attempting to push unregistered frame type %s", GetType());
      } // guess we'll just go with it, though

      lua_setmetatable(L, -3);

      // stack: ... newtab Frames_mt

      // Pop Frames_mt
      lua_pop(L, 1);
      
      // Push Frames_rg
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rg");

      // stack: ... newtab Frames_rg
      // Now we call our deep virtual register function - this leaves the stack at the same level
      {
        Environment::LuaStackChecker(L, m_env);
        l_Register(L);
      }

      // Now we're all registered in both lookup tables, yay

      // Strip out the _rg
      lua_pop(L, 1);

      // And we're done!
    }
  }

  std::string Layout::GetNameDebug() const {
    std::string name;
    if (m_parent) {
      name = m_parent->GetNameDebug() + ".";
    }

    bool delimiter = false;

    name += "(";

    if (!m_name_dynamic.empty()) {
      if (delimiter) name += ":";
      name += m_name_dynamic;
      delimiter = true;
    }

    if (m_name_static) {
      if (delimiter) name += ":";
      name += m_name_static;
      delimiter = true;
    }

    if (m_name_id != -1) {
      if (delimiter) name += ":";
      name += Utility::Format("%d", m_name_id);
      delimiter = true;
    }

    if (!delimiter) {
      // well okay then
      name += Utility::Format("%08x", this);
      delimiter = true;
    }

    name += ")";

    return name;
  }
}
#include <windows.h>
namespace Frames {
  Layout::Layout(Layout *layout, Environment *env) :
      m_resolved(false),
      m_layer(0),
      m_strata(0),
      m_visible(true),
      m_name_static(0),
      m_name_id(-1),
      m_env(0),
      m_obliterating(false),
      m_parent(0),
      m_last_width(-1),
      m_last_height(-1),
      m_last_x(-1),
      m_last_y(-1)
  {
    if (layout) {
      m_env = layout->GetEnvironment();
    } else {
      m_env = env;
    }

    if (layout && env) {
      FRAMES_LAYOUT_CHECK(layout->GetEnvironment() == env, "Layout's explicit parent and environment do not match");
    }
    // TODO: come up with a better panic button for this? if we have no parent or environment then we have no way to do debug loggin
    FRAMES_LAYOUT_CHECK(layout || env, "Layout not given parent or environment");

    m_env->MarkInvalidated(this); // We'll need to resolve this before we go

    if (layout) {
      SetParent(layout);
    }
  }

  Layout::~Layout() {
    // We shouldn't reach this point until all references to us have vanished, so, kapow!
    FRAMES_LAYOUT_CHECK(Utility::IsUndefined(m_axes[X].connections[0].point_mine), "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(Utility::IsUndefined(m_axes[X].connections[1].point_mine), "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(Utility::IsUndefined(m_axes[Y].connections[0].point_mine), "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(Utility::IsUndefined(m_axes[Y].connections[1].point_mine), "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(!m_parent, "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(m_children.empty(), "Layout destroyed while still connected");
  }

  void Layout::SetPoint(Axis axis, float mypt, const Layout *link, float linkpt, float offset) {
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
      return;
    }

    if (!axau && !axbu) {
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

    if (!Utility::IsUndefined(ax.connections[0].point_mine) && !Utility::IsUndefined(ax.connections[1].point_mine)) {
      return;
    }

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

  void Layout::ClearLayout() {
    ClearSize(X);
    ClearSize(Y);

    ClearAllPoints(X);
    ClearAllPoints(Y);
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

  void Layout::SetParent(Layout *layout) {
    if (m_parent == layout) {
      return;
    }

    if (!layout) {
      FRAMES_LAYOUT_CHECK(false, ":SetParent() attempted with null parent");
      return;
    }

    if (layout && m_env != layout->GetEnvironment()) {
      FRAMES_LAYOUT_CHECK(false, ":SetParent() attempted across environment boundaries");
      return;
    }

    // First, remove ourselves from our old parent
    if (m_parent) {
      m_parent->m_children.erase(this);
    }

    m_parent = layout;

    m_parent->m_children.insert(this);
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

  void Layout::Obliterate() {
    Obliterate_Detach();
    Obliterate_Extract();
  }

  void Layout::l_RegisterWorker(lua_State *L, const char *name) const {
    Environment::LuaStackChecker(L, m_env);
    // Incoming: ... newtab Frames_rg

    lua_getfield(L, -1, name);

    FRAMES_LAYOUT_ASSERT(!lua_isnil(L, -1), "Attempted to register frame as %s without that type being registered in Lua", name);
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);
      return;
    }

    lua_pushvalue(L, -3);
    lua_pushlightuserdata(L, (void*)this);
    lua_rawset(L, -3);

    lua_pop(L, 1);
  }

  void Layout::Render(Renderer *renderer) const {
    if (m_visible) {
      RenderElement(renderer);

      for (ChildrenList::const_iterator itr = m_children.begin(); itr != m_children.end(); ++itr) {
        (*itr)->Render(renderer);
      }
      
      RenderElementPost(renderer);
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
        m_env->MarkInvalidated(this);
      }
    }
  }

  void Layout::Obliterate_Detach() {
    ClearLayout();  // kill my layout

    // OBLITERATE ALL CHILDREN.
    for (ChildrenList::const_iterator itr = m_children.begin(); itr != m_children.end(); ++itr) {
      (*itr)->Obliterate_Detach();
    }
  }

  void Layout::Obliterate_Extract() {
    // at this point, nobody should be referring to me, in theory
    Obliterate_Extract_Axis(X);
    Obliterate_Extract_Axis(Y);

    // OBLITERATE ALL CHILDREN.
    for (ChildrenList::const_iterator itr = m_children.begin(); itr != m_children.end(); ) {
      // little dance here 'cause our children are going to be fucking around with our structure
      ChildrenList::const_iterator next = itr;
      ++next;
      (*itr)->Obliterate_Extract();
      itr = next;
    }

    // Detach ourselves from our parent
    if (m_parent) {
      m_parent->m_children.erase(this);
    }
    m_parent = 0;

    // And now we're safe to delete ourselves
    if (m_resolved) {
      delete this;
    } else {
      m_obliterating = true;  // mark ourselves for obliterate - it's this or do something gnarly with the invalidation system
    }
  }

  void Layout::Obliterate_Extract_Axis(Axis axis) {
    const AxisData &ax = m_axes[axis];
    while (!ax.children.empty()) {
      Layout *layout = *ax.children.begin();
      layout->Obliterate_Extract_From(axis, this);
    }
  }

  void Layout::Obliterate_Extract_From(Axis axis, const Layout *layout) {
    const AxisData &ax = m_axes[axis];

    if (ax.connections[0].link == layout) {
      FRAMES_LAYOUT_ASSERT(false, "Obliterated frame %s is still referenced by active frame %s on axis %c/%f, clearing link", layout->GetNameDebug().c_str(), GetNameDebug().c_str(), axis ? 'Y' : 'X', ax.connections[0].point_mine);
      ClearPoint(axis, ax.connections[0].point_mine);
    }

    if (ax.connections[1].link == layout) {
      FRAMES_LAYOUT_ASSERT(false, "Obliterated frame %s is still referenced by active frame %s on axis %c/%f, clearing link", layout->GetNameDebug().c_str(), GetNameDebug().c_str(), axis ? 'Y' : 'X', ax.connections[1].point_mine);
      ClearPoint(axis, ax.connections[1].point_mine);
    }
  }

  void Layout::Resolve() const {
    if (m_obliterating) {
      delete this; // that was the last reference needed, we're set
      return;
    }

    float nx = GetLeft();
    GetRight();
    float ny = GetTop();
    GetBottom();
    
    float nw = GetWidth();
    float nh = GetHeight();

    m_resolved = true;

    bool sizechange = (nw != m_last_width || nh != m_last_height);
    bool movechange = (nx != m_last_x || ny != m_last_y);

    m_last_width = nw;
    m_last_height = nh;
    m_last_x = nx;
    m_last_y = ny;

    if (sizechange) {
      EventSizeTrigger();
    }

    if (sizechange || movechange) {
      EventMoveTrigger();
    }

    // Todo: queue up movement events
  }

  bool Layout::Sorter::operator()(const Layout *lhs, const Layout *rhs) const {
    if (lhs->GetStrata() != rhs->GetStrata())
      return lhs->GetStrata() < rhs->GetStrata();
    if (lhs->GetLayer() != rhs->GetLayer())
      return lhs->GetLayer() < rhs->GetLayer();
    // they're the same, but we want a consistent sort that won't result in Z-fighting
    return lhs < rhs;
  }


  class Layout::EventHandler {
  public:
    // NOTE: this works because delegate is POD
    EventHandler() { };
    EventHandler(Delegate<void ()> din) {
      typedef Delegate<void ()> dintype;
      BOOST_STATIC_ASSERT(sizeof(dintype) == sizeof(Delegate<void ()>));

      *reinterpret_cast<dintype *>(c.delegate) = din;
    };
    ~EventHandler() { };

    void Call() const {
      (*reinterpret_cast<const Delegate<void ()> *>(c.delegate))();
    };

  private:
    union {
      char delegate[sizeof(Delegate<void ()>)];
    } c;
    
    friend bool operator==(const EventHandler &lhs, const EventHandler &rhs);
  };

  bool operator==(const Layout::EventHandler &lhs, const Layout::EventHandler &rhs) { return memcmp(&lhs.c, &rhs.c, sizeof(lhs.c)) == 0; }

  // eventery
  #define FRAMES_LAYOUT_EVENT_DEFINE(frametype, eventname, paramlist, params) \
    void frametype::Event##eventname##Attach(Delegate<void paramlist> delegate, float order) { \
      EventAttach((unsigned int)&s_event_##eventname##_id, EventHandler(delegate), order); \
    } \
    void frametype::Event##eventname##Detach(Delegate<void paramlist> delegate) { \
      EventDetach((unsigned int)&s_event_##eventname##_id, EventHandler(delegate)); \
    } \
    void frametype::Event##eventname##Trigger paramlist const { \
      std::map<unsigned int, std::multimap<float, EventHandler> >::const_iterator itr = m_events.find((unsigned int)&s_event_##eventname##_id); \
      if (itr == m_events.end()) { return; } \
      const std::multimap<float, EventHandler> &tab = itr->second; \
      for (std::multimap<float, EventHandler>::const_iterator ev = tab.begin(); ev != tab.end(); ++ev) { \
        ev->second.Call params; \
      } \
    } \
    char frametype::s_event_##eventname##_id = 0;

  FRAMES_LAYOUT_EVENT_DEFINE(Layout, Move, (), ());
  FRAMES_LAYOUT_EVENT_DEFINE(Layout, Size, (), ());

  void Layout::EventAttach(unsigned int id, const EventHandler &handler, float order) {
    m_events[id].insert(std::make_pair(order, handler)); // kapowza!
  }
  void Layout::EventDetach(unsigned int id, const EventHandler &handler) {
    // somewhat more complex, we need to actually find the handler
    std::map<unsigned int, std::multimap<float, EventHandler> >::iterator itr = m_events.find(id);
    if (itr == m_events.end()) {
      return;
    }

    std::multimap<float, EventHandler> &tab = itr->second;

    for (std::multimap<float, EventHandler>::iterator ev = tab.begin(); ev != tab.end(); ++ev) {
      if (ev->second == handler) {
        tab.erase(ev);
        break;
      }
    }
  }

}

