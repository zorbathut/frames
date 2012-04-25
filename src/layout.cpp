
#include "frames/layout.h"

#include "frames/environment.h"
#include "frames/lua.h"
#include "frames/rect.h"
#include "frames/renderer.h"

#include "boost/static_assert.hpp"

#include <math.h> // just for isnan()

namespace Frames {
  BOOST_STATIC_ASSERT(sizeof(EventId) == sizeof(intptr_t));
  BOOST_STATIC_ASSERT(sizeof(EventId) == sizeof(void *));

  class Layout::EventHandler {
  public:
    // NOTE: this works because delegate is POD
    EventHandler() { }
    template<typename T> EventHandler(Delegate<T> din) : type(TYPE_NATIVE) {
      typedef Delegate<T> dintype;
      BOOST_STATIC_ASSERT(sizeof(dintype) == sizeof(Delegate<void ()>));

      *reinterpret_cast<dintype *>(c.delegate) = din;
    }
    template<typename T> EventHandler(Delegate<T> din, const LuaFrameEventHandler *lua) : type(TYPE_LUA), lua(lua) {
      typedef Delegate<T> dintype;
      BOOST_STATIC_ASSERT(sizeof(dintype) == sizeof(Delegate<void ()>));

      *reinterpret_cast<dintype *>(c.delegate) = din;
    }
    ~EventHandler() { }

    template<typename T1> void Call(T1 t1) const {
      (*reinterpret_cast<const Delegate<void (T1)> *>(c.delegate))(t1);
    }

    template<typename T1, typename T2> void Call(T1 t1, T2 t2) const {
      (*reinterpret_cast<const Delegate<void (T1, T2)> *>(c.delegate))(t1, t2);
    }

    template<typename T1, typename T2, typename T3> void Call(T1 t1, T2 t2, T3 t3) const {
      (*reinterpret_cast<const Delegate<void (T1, T2, T3)> *>(c.delegate))(t1, t2, t3);
    }

    template<typename T1, typename T2, typename T3, typename T4> void Call(T1 t1, T2 t2, T3 t3, T4 t4) const {
      (*reinterpret_cast<const Delegate<void (T1, T2, T3, T4)> *>(c.delegate))(t1, t2, t3, t4);
    }

    bool IsNative() const { return type == TYPE_NATIVE; }

    bool IsLua() const { return type == TYPE_LUA; }
    const LuaFrameEventHandler *GetLua() const { return lua; }

  private:
    union {
      char delegate[sizeof(Delegate<void ()>)];
    } c;

    enum { TYPE_NATIVE, TYPE_LUA } type;

    union {
      const LuaFrameEventHandler *lua;
    };
    
    friend bool operator==(const EventHandler &lhs, const EventHandler &rhs);
  };

  template<typename F> struct EventHandlerCaller;
  template<> struct EventHandlerCaller<void ()> { static void Call(const Layout::EventHandler *ev, EventHandle *eh) { ev->Call<EventHandle *>(eh); } };
  template<typename P1> struct EventHandlerCaller<void (P1)> { static void Call(const Layout::EventHandler *ev, EventHandle *eh, P1 p1) { ev->Call<EventHandle *, P1>(eh, p1); } };
  template<typename P1, typename P2> struct EventHandlerCaller<void (P1, P2)> { static void Call(const Layout::EventHandler *ev, EventHandle *eh, P1 p1, P2 p2) { ev->Call<EventHandle *, P1, P2>(eh, p1, p2); } };
  template<typename P1, typename P2, typename P3> struct EventHandlerCaller<void (P1, P2, P3)> { static void Call(const Layout::EventHandler *ev, EventHandle *eh, P1 p1, P2 p2, P3 p3) { ev->Call<EventHandle *, P1, P2, P3>(eh, p1, p2, p3); } };
  template<typename P1, typename P2, typename P3, typename P4> struct EventHandlerCaller<void (P1, P2, P3, P4)> { static void Call(const Layout::EventHandler *ev, EventHandle *eh, P1 p1, P2 p2, P3 p3, P4 p4) { ev->Call<EventHandle *, P1, P2, P3, P4>(eh, p1, p2, p3, p4); } };

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

  Layout *Layout::GetFrameUnder(int x, int y) {
    if (!GetVisible()) return 0; // nope

    if (m_fullMouseMasking && !TestMouseMasking(x, y)) return 0;

    for (ChildrenList::const_reverse_iterator itr = m_children.rbegin(); itr != m_children.rend(); ++itr) {
      Layout *prv = (*itr)->GetFrameUnder(x, y);
      if (prv) return prv;
    }

    if (m_acceptInput && x >= GetLeft() && y >= GetTop() && x < GetRight() && y < GetBottom()) {
      return this;
    }

    return 0;
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
      // * verify that this environment has been pushed
      // * generate an appropriate table, with attached metatable
      // * register that table in _rrg
      // * register that table in all appropriate _rg's
      
      // first off, we'll need to kill the nil we just pulled out
      lua_pop(L, 1);

      // let's verify that our environment has been registered properly
      // maybe in the future we should just automatically register?
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_env");
      lua_pushlightuserdata(L, m_env);
      lua_rawget(L, -2);
      if (lua_isnil(L, -1)) {
        // fffff
        FRAMES_ERROR("Frames environment not registered with this lua environment");

        // Let's try to register it - this is probably the best recovery we can hope for, otherwise we're opening up some security holes
        m_env->LuaRegister(L);
      }
      lua_pop(L, 2);  // clean up

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

  std::string Layout::GetName() const {
    std::string name;
    bool delimiter = false;

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
      name += Utility::Format("%p", this);
      delimiter = true;
    }

    return name;
  }

  std::string Layout::GetNameFull() const {
    std::string name;
    if (m_parent) {
      name = m_parent->GetNameFull() + ".";
    }

    return name + "(" + GetName() + ")";
  }
}
#include <windows.h>
namespace Frames {
  Layout::Layout(Layout *layout, Environment *env) :
      m_resolved(false),
      m_last_width(-1),
      m_last_height(-1),
      m_last_x(-1),
      m_last_y(-1),
      m_layer(0),
      m_strata(0),
      m_parent(0),
      m_visible(true),
      m_fullMouseMasking(false),
      m_acceptInput(false),
      m_name_static(0),
      m_name_id(-1),
      m_env(0)
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

    // Clear ourselves out of the resolved todo
    if (!m_resolved) {
      m_env->UnmarkInvalidated(this);
    }

    // Clean up all appropriate lua environments
    for (std::set<lua_State *>::const_iterator itr = m_env->m_lua_environments.begin(); itr != m_env->m_lua_environments.end(); ++itr) {
      l_ClearLuaEvents(*itr); // We can be a little more clever about this if necessary, in a lot of ways. Again, deal with later, if necessary.

      lua_State *L = *itr;

      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rrg");
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rg");

      lua_pushlightuserdata(L, this);
      lua_pushlightuserdata(L, this);

      lua_rawget(L, -4);

      // Stack: ... Frames_rrg Frames_rg userdata table

      lua_pushnil(L);
      // Stack: ... Frames_rrg Frames_rg userdata table key
      while (lua_next(L, -4) != 0) {
        // we're just blowing through all types, it's easier and this is likely not a speed issue
        // Stack: ... Frames_rrg Frames_rg userdata table key value
        lua_pushvalue(L, -3);
        lua_pushnil(L);
        lua_rawset(L, -3);

        lua_pop(L, 1);
        // Stack: ... Frames_rrg Frames_rg userdata table key
      }

      // Stack: ... Frames_rrg Frames_rg userdata table
      lua_pop(L, 1);
      lua_pushnil(L);
      lua_rawset(L, -4);

      // Stack: ... Frames_rrg Frames_rg
      lua_pop(L, 2);
    }

    // Notify the environment
    m_env->DestroyingLayout(this);
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

  const Layout::EventMap *Layout::GetEventTable(EventId id) const {
    std::map<EventId, EventMap>::const_iterator itr = m_events.find(id);
    if (itr != m_events.end()) {
      return &itr->second;
    } else {
      return 0;
    }
  }

  void Layout::EventAttached(EventId id) {
    m_acceptInput =
      GetEventTable(EventMouseLeftClickId()) || GetEventTable(EventMouseLeftUpId()) || GetEventTable(EventMouseLeftDownId()) ||
      GetEventTable(EventMouseMiddleClickId()) || GetEventTable(EventMouseMiddleUpId()) || GetEventTable(EventMouseMiddleDownId()) ||
      GetEventTable(EventMouseRightClickId()) || GetEventTable(EventMouseRightUpId()) || GetEventTable(EventMouseRightDownId()) ||
      GetEventTable(EventMouseButtonClickId()) || GetEventTable(EventMouseButtonUpId()) || GetEventTable(EventMouseButtonDownId()) ||
      GetEventTable(EventMouseWheelId()) || GetEventTable(EventMouseOverId()) || GetEventTable(EventMouseOutId());
  }

  void Layout::EventDetached(EventId id) {
    m_acceptInput =
      GetEventTable(EventMouseLeftClickId()) || GetEventTable(EventMouseLeftUpId()) || GetEventTable(EventMouseLeftDownId()) ||
      GetEventTable(EventMouseMiddleClickId()) || GetEventTable(EventMouseMiddleUpId()) || GetEventTable(EventMouseMiddleDownId()) ||
      GetEventTable(EventMouseRightClickId()) || GetEventTable(EventMouseRightUpId()) || GetEventTable(EventMouseRightDownId()) ||
      GetEventTable(EventMouseButtonClickId()) || GetEventTable(EventMouseButtonUpId()) || GetEventTable(EventMouseButtonDownId()) ||
      GetEventTable(EventMouseWheelId()) || GetEventTable(EventMouseOverId()) || GetEventTable(EventMouseOutId());
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

  #define L_REGISTEREVENT(L, GST, name, handler) \
    l_RegisterEvent<Event##name##Functiontype>(L, GST, "Event" #name "Attach", "Event" #name "Detach", Event##name##Id(), static_cast<typename LayoutHandlerMunger<Event##name##Functiontype>::T>(handler));

  #define L_REGISTEREVENT_BUBBLE(L, GST, name, handler) \
    L_REGISTEREVENT(L, GST, name, handler); \
    L_REGISTEREVENT(L, GST, name##Sink, handler); \
    L_REGISTEREVENT(L, GST, name##Bubble, handler);

  /*static*/ void Layout::l_RegisterFunctions(lua_State *L) {
    l_RegisterFunction(L, GetStaticType(), "GetLeft", l_GetLeft);
    l_RegisterFunction(L, GetStaticType(), "GetRight", l_GetRight);
    l_RegisterFunction(L, GetStaticType(), "GetTop", l_GetTop);
    l_RegisterFunction(L, GetStaticType(), "GetBottom", l_GetBottom);
    l_RegisterFunction(L, GetStaticType(), "GetBounds", l_GetBounds);

    l_RegisterFunction(L, GetStaticType(), "GetWidth", l_GetWidth);
    l_RegisterFunction(L, GetStaticType(), "GetHeight", l_GetHeight);

    l_RegisterFunction(L, GetStaticType(), "GetChildren", l_GetChildren);

    l_RegisterFunction(L, GetStaticType(), "GetName", l_GetName);
    l_RegisterFunction(L, GetStaticType(), "GetNameFull", l_GetNameFull);
    l_RegisterFunction(L, GetStaticType(), "GetType", l_GetType);

    L_REGISTEREVENT(L, GetStaticType(), Move, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT(L, GetStaticType(), Size, &Layout::l_EventPusher_Default);

    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseOver, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseMove, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseMoveOutside, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseOut, &Layout::l_EventPusher_Default);

    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseLeftUp, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseLeftUpOutside, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseLeftDown, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseLeftClick, &Layout::l_EventPusher_Default);

    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseMiddleUp, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseMiddleUpOutside, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseMiddleDown, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseMiddleClick, &Layout::l_EventPusher_Default);

    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseRightUp, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseRightUpOutside, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseRightDown, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseRightClick, &Layout::l_EventPusher_Default);

    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseButtonUp, &Layout::l_EventPusher_Button);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseButtonUpOutside, &Layout::l_EventPusher_Button);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseButtonDown, &Layout::l_EventPusher_Button);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseButtonClick, &Layout::l_EventPusher_Button);

    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), MouseWheel, &Layout::l_EventPusher_Default);

    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), KeyDown, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), KeyType, &Layout::l_EventPusher_Default);
    L_REGISTEREVENT_BUBBLE(L, GetStaticType(), KeyUp, &Layout::l_EventPusher_Default);
  }

  /*static*/ void Layout::l_RegisterFunction(lua_State *L, const char *owner, const char *name, int (*func)(lua_State *)) {
    // From Environment::RegisterLuaFrame
    // Stack: ... Frames_mt Frames_rg Frames_fev Frames_rfev Frames_cfev metatable indexes
    lua_getfield(L, -6, owner);
    lua_pushcclosure(L, func, 1);
    lua_setfield(L, -2, name);
  }

  BOOST_STATIC_ASSERT(sizeof(intptr_t) >= sizeof(void*));
  template<typename Prototype> /*static*/ void Layout::l_RegisterEvent(lua_State *L, const char *owner, const char *nameAttach, const char *nameDetach, EventId eventId, typename LayoutHandlerMunger<Prototype>::T pusher) {
    BOOST_STATIC_ASSERT(sizeof(Utility::intfptr_t) == sizeof(intptr_t)); // if this fails, we can do something clever with real lua userdata instead
    BOOST_STATIC_ASSERT(sizeof(Utility::intfptr_t) == sizeof(void*)); // if this fails, we can do something clever with real lua userdata instead
    BOOST_STATIC_ASSERT(sizeof(Utility::intfptr_t) >= sizeof(typename Utility::FunctionPtr<typename Utility::FunctionPrefix<lua_State *, Prototype>::T>::T)); // if this fails we are kind of screwed

    // From Environment::RegisterLuaFrame
    // Stack: ... Frames_mt Frames_rg Frames_fev Frames_rfev Frames_cfev metatable indexes
    lua_getfield(L, -6, owner);
    lua_pushlightuserdata(L, (void*)eventId); // what was once pointer will now be again pointer
    lua_pushvalue(L, -7); // push _fev
    lua_pushvalue(L, -7); // push _rfev
    lua_pushvalue(L, -7); // push _cfev
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_lua"); // push lua root
    lua_pushlightuserdata(L, (void*)pusher); // push handler function
    lua_pushcclosure(L, l_RegisterEventAttach<Prototype>, 7);
    lua_setfield(L, -2, nameAttach);

    // DO IT AGAIN
    lua_getfield(L, -6, owner);
    lua_pushlightuserdata(L, (void*)eventId); // what was once pointer will now be again pointer
    lua_pushvalue(L, -7); // push _fev
    lua_pushvalue(L, -7); // push _rfev
    lua_pushvalue(L, -7); // push _cfev
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_lua"); // push lua root
    lua_pushlightuserdata(L, (void*)pusher); // push handler function
    lua_pushcclosure(L, l_RegisterEventDetach<Prototype>, 7);
    lua_setfield(L, -2, nameDetach);
  }

  template<typename Prototype> /*static*/ int Layout::l_RegisterEventAttach(lua_State *L) {
    l_checkparams(L, 2, 3);
    Layout *self = l_checkframe<Layout>(L, 1);

    // Stack: table handler (priority)
    // Upvalues: tableregistry eventid _fev _rfev _cfev luaroot handler

    float priority = (float)luaL_optnumber(L, 3, 0.f);
    lua_settop(L, 2);

    // Stack: table handler

    // Look up this element in our reverse frame lookup
    lua_pushvalue(L, -1);
    lua_rawget(L, lua_upvalueindex(4));
    int idx = 0;
    if (!lua_isnil(L, -1)) {
      // this already exists, so let's increment our refcount
      // Stack: table handler id
      idx = (int)lua_tonumber(L, -1);
      lua_pop(L, 1);
      // Stack: table handler

      // Update count
      lua_rawgeti(L, lua_upvalueindex(5), idx);
      lua_pushnumber(L, lua_tonumber(L, -1) + 1);
      lua_rawseti(L, lua_upvalueindex(5), idx);
      lua_pop(L, 1);

      lua_pop(L, 1);
      // Stack: table
    } else {
      // doesn't already exist, so do all our setup
      lua_pop(L, 1);
      // Stack: table handler

      // Forward table, get our index
      lua_pushvalue(L, -1);
      idx = luaL_ref(L, lua_upvalueindex(3));
      // Stack: table handler

      // Reverse table
      lua_pushnumber(L, idx);
      lua_rawset(L, lua_upvalueindex(4));
      // Stack: table

      // Count table
      lua_pushnumber(L, 1);
      lua_rawseti(L, lua_upvalueindex(5), idx);
      // Stack: table
    }
    // One way or another, our tables are maintained properly now
    // Stack: table

    EventId event = (EventId)lua_touserdata(L, lua_upvalueindex(2));

    // Finally, we need the "root" lua environment
    lua_State *L_root = (lua_State *)lua_touserdata(L, lua_upvalueindex(6));
    
    // We've got the root lua, a layout, the event, the priority, and a valid index to a handler. Yahoy!
    self->l_EventAttach<Prototype>(L_root, event, idx, priority, reinterpret_cast<typename LayoutHandlerMunger<Prototype>::T>(lua_touserdata(L, lua_upvalueindex(7))));

    return 0;
  }

  template<typename Prototype> /*static*/ int Layout::l_RegisterEventDetach(lua_State *L) {
    l_checkparams(L, 2, 3);
    Layout *self = l_checkframe<Layout>(L, 1);

    // Stack: table handler (priority)
    // Upvalues: tableregistry eventid _fev _rfev _cfev luaroot

    float priority = (float)luaL_optnumber(L, 3, 0.f / 0.f);
    lua_settop(L, 2);

    // Stack: table handler
  // Look up this element in our reverse frame lookup
    lua_pushvalue(L, -1);
    lua_rawget(L, lua_upvalueindex(4));
    if (lua_isnil(L, -1)) {
      // this doesn't exist! we're done!
      return 0;
    }

    int idx = (int)lua_tonumber(L, -1);

    EventId event = (EventId)lua_touserdata(L, lua_upvalueindex(2));

    // Finally, we need the "root" lua environment
    lua_State *L_root = (lua_State *)lua_touserdata(L, lua_upvalueindex(6));

    // We've got the root lua, a layout, the event, the priority, and a valid index to a handler. Yahoy!
    self->l_EventDetach<Prototype>(L_root, event, idx, priority);

    return 0;
  }

  struct EventhandlerInfo {
    Layout::EventHandler *eh;
    EventId event;
    float priority;
    bool operator<(const EventhandlerInfo &rhs) const { return eh < rhs.eh; }
  };

  void Layout::l_ClearLuaEvents(lua_State *lua) {
    // clear out all events attached to this lua state
    // this could probably be more efficient - if this is somehow a bottleneck or a speed issue, we can fix it!
    std::set<EventhandlerInfo> eh;
    for (std::map<EventId, EventMap>::iterator itr = m_events.begin(); itr != m_events.end(); ++itr) {
      for (EventMap::iterator eitr = itr->second.begin(); eitr != itr->second.end(); ++eitr) {
        if (eitr->second.IsLua() && eitr->second.GetLua()->L == lua) {
          EventhandlerInfo ehi;
          ehi.eh = &eitr->second;
          ehi.priority = eitr->first;
          ehi.event = itr->first;
          eh.insert(ehi);
        }
      }
    }

    for (std::set<EventhandlerInfo>::iterator itr = eh.begin(); itr != eh.end(); ++itr) {
      if (!l_EventDetach(lua, m_lua_events.find(*itr->eh->GetLua()), itr->event, *itr->eh, itr->priority)) {
        FRAMES_ERROR("Failing to shutdown Lua events properly");
      }
    }
  }
  void Layout::l_ClearLuaEvents_Recursive(lua_State *lua) {
    l_ClearLuaEvents(lua);

    // get children to clear events as well
    for (ChildrenList::iterator itr = m_children.begin(); itr != m_children.end(); ++itr) {
      (*itr)->l_ClearLuaEvents(lua);
    }
  }

  /*static*/ int Layout::l_EventPusher_Default(lua_State *L) {
    return 0;
  }

  /*static*/ int Layout::l_EventPusher_Default(lua_State *L, int p1) {
    lua_pushnumber(L, p1);
    return 1;
  }

  /*static*/ int Layout::l_EventPusher_Default(lua_State *L, const std::string &p1) {
    lua_pushstring(L, p1.c_str());
    return 1;
  }

  /*static*/ int Layout::l_EventPusher_Default(lua_State *L, const Point &pt) {
    lua_pushnumber(L, pt.x);
    lua_pushnumber(L, pt.y);
    return 2;
  }

   /*static*/ int Layout::l_EventPusher_Default(lua_State *L, const KeyEvent &p1) {
    lua_pushnumber(L, p1.key);
    lua_newtable(L);
    if (p1.shift) {
      lua_pushboolean(L, true);
      lua_setfield(L, -2, "shift");
    }
    if (p1.alt) {
      lua_pushboolean(L, true);
      lua_setfield(L, -2, "alt");
    }
    if (p1.ctrl) {
      lua_pushboolean(L, true);
      lua_setfield(L, -2, "ctrl");
    }
    return 2;
  }

  /*static*/ int Layout::l_EventPusher_Button(lua_State *L, int button) {
    lua_pushnumber(L, button + 1);
    return 1;
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
    delete this;
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
      FRAMES_LAYOUT_ASSERT(false, "Obliterated frame %s is still referenced by active frame %s on axis %c/%f, clearing link", layout->GetNameFull().c_str(), GetNameFull().c_str(), axis ? 'Y' : 'X', ax.connections[0].point_mine);
      ClearPoint(axis, ax.connections[0].point_mine);
    }

    if (ax.connections[1].link == layout) {
      FRAMES_LAYOUT_ASSERT(false, "Obliterated frame %s is still referenced by active frame %s on axis %c/%f, clearing link", layout->GetNameFull().c_str(), GetNameFull().c_str(), axis ? 'Y' : 'X', ax.connections[1].point_mine);
      ClearPoint(axis, ax.connections[1].point_mine);
    }
  }

  void Layout::Resolve() const {
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

  bool operator==(const Layout::EventHandler &lhs, const Layout::EventHandler &rhs) { return memcmp(&lhs.c, &rhs.c, sizeof(lhs.c)) == 0; }

  // eventery
  #define FLEPREPARAMETERS &ev->second, &handle

  #define FRAMES_LAYOUT_EVENT_DEFINE_INFRA(frametype, eventname, paramlist, paramlistcomplete, params) \
    void frametype::Event##eventname##Attach(Delegate<void paramlistcomplete> delegate, float order /*= 0.f*/) { \
      EventAttach(Event##eventname##Id(), EventHandler(delegate), order); \
    } \
    void frametype::Event##eventname##Detach(Delegate<void paramlistcomplete> delegate, float order /*= 0.f / 0.f*/) { \
      EventDetach(Event##eventname##Id(), EventHandler(delegate), order); \
    } \
    /*static*/ EventId frametype::Event##eventname##Id() { \
      return (EventId)&s_event_##eventname##_id; \
    } \
    char frametype::s_event_##eventname##_id = 0;

  #define FRAMES_LAYOUT_EVENT_DEFINE(frametype, eventname, paramlist, paramlistcomplete, params) \
    FRAMES_LAYOUT_EVENT_DEFINE_INFRA(frametype, eventname, paramlist, paramlistcomplete, params) \
    void frametype::Event##eventname##Trigger paramlist const { \
      std::map<EventId, std::multimap<float, EventHandler> >::const_iterator itr = m_events.find(Event##eventname##Id()); \
      if (itr != m_events.end()) { \
        EventHandle handle; \
        const std::multimap<float, EventHandler> &tab = itr->second; \
        for (std::multimap<float, EventHandler>::const_iterator ev = tab.begin(); ev != tab.end(); ++ev) { \
          EventHandlerCaller<Event##eventname##Functiontype>::Call params; \
        } \
      } \
    }

  #define FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(frametype, eventname, paramlist, paramlistcomplete, params) \
    FRAMES_LAYOUT_EVENT_DEFINE_INFRA(frametype, eventname, paramlist, paramlistcomplete, params) \
    FRAMES_LAYOUT_EVENT_DEFINE_INFRA(frametype, eventname##Sink, paramlist, paramlistcomplete, params) \
    FRAMES_LAYOUT_EVENT_DEFINE_INFRA(frametype, eventname##Bubble, paramlist, paramlistcomplete, params) \
    void frametype::Event##eventname##Trigger paramlist const { \
      std::vector<Layout *> layouts; \
      Layout *parent = GetParent(); \
      while (parent) { \
        layouts.push_back(parent); \
        parent = parent->GetParent(); \
      } \
      EventHandle handle; \
      for (int i = (int)layouts.size() - 1; i >= 0; --i) { \
        std::map<EventId, std::multimap<float, EventHandler> >::const_iterator itr = layouts[i]->m_events.find(Event##eventname##Sink##Id()); \
        if (itr != layouts[i]->m_events.end()) { \
          const std::multimap<float, EventHandler> &tab = itr->second; \
          for (std::multimap<float, EventHandler>::const_iterator ev = tab.begin(); ev != tab.end(); ++ev) { \
            EventHandlerCaller<Event##eventname##Functiontype>::Call params; \
          } \
        } \
      } \
      { \
        std::map<EventId, std::multimap<float, EventHandler> >::const_iterator itr = m_events.find(Event##eventname##Id()); \
        if (itr != m_events.end()) { \
          const std::multimap<float, EventHandler> &tab = itr->second; \
          for (std::multimap<float, EventHandler>::const_iterator ev = tab.begin(); ev != tab.end(); ++ev) { \
            EventHandlerCaller<Event##eventname##Functiontype>::Call params; \
          } \
        } \
      } \
      for (int i = 0; i < (int)layouts.size(); ++i) { \
        std::map<EventId, std::multimap<float, EventHandler> >::const_iterator itr = layouts[i]->m_events.find(Event##eventname##Bubble##Id()); \
        if (itr != layouts[i]->m_events.end()) { \
          const std::multimap<float, EventHandler> &tab = itr->second; \
          for (std::multimap<float, EventHandler>::const_iterator ev = tab.begin(); ev != tab.end(); ++ev) { \
            EventHandlerCaller<Event##eventname##Functiontype>::Call params; \
          } \
        } \
      } \
    }

  FRAMES_LAYOUT_EVENT_DEFINE(Layout, Move, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE(Layout, Size, (), (EventHandle *handle), (FLEPREPARAMETERS));

  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseOver, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseMove, (const Point &pt), (EventHandle *handle, const Point &pt), (FLEPREPARAMETERS, pt));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseMoveOutside, (const Point &pt), (EventHandle *handle, const Point &pt), (FLEPREPARAMETERS, pt));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseOut, (), (EventHandle *handle), (FLEPREPARAMETERS));

  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseLeftUp, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseLeftUpOutside, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseLeftDown, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseLeftClick, (), (EventHandle *handle), (FLEPREPARAMETERS));

  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseMiddleUp, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseMiddleUpOutside, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseMiddleDown, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseMiddleClick, (), (EventHandle *handle), (FLEPREPARAMETERS));

  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseRightUp, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseRightUpOutside, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseRightDown, (), (EventHandle *handle), (FLEPREPARAMETERS));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseRightClick, (), (EventHandle *handle), (FLEPREPARAMETERS));

  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseButtonUp, (int button), (EventHandle *handle, int button), (FLEPREPARAMETERS, button));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseButtonUpOutside, (int button), (EventHandle *handle, int button), (FLEPREPARAMETERS, button));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseButtonDown, (int button), (EventHandle *handle, int button), (FLEPREPARAMETERS, button));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseButtonClick, (int button), (EventHandle *handle, int button), (FLEPREPARAMETERS, button));

  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, MouseWheel, (int delta), (EventHandle *handle, int delta), (FLEPREPARAMETERS, delta));

  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, KeyDown, (const KeyEvent &kev), (EventHandle *event, const KeyEvent &kev), (FLEPREPARAMETERS, kev));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, KeyType, (const std::string &text), (EventHandle *event, const std::string &text), (FLEPREPARAMETERS, text));
  FRAMES_LAYOUT_EVENT_DEFINE_BUBBLE(Layout, KeyUp, (const KeyEvent &kev), (EventHandle *event, const KeyEvent &kev), (FLEPREPARAMETERS, kev));

  void Layout::EventAttach(EventId id, const EventHandler &handler, float order) {
    m_events[id].insert(std::make_pair(order, handler)); // kapowza!
    EventAttached(id);
  }
  bool Layout::EventDetach(EventId id, const EventHandler &handler, float order) {
    // somewhat more complex, we need to actually find the handler
    std::map<EventId, std::multimap<float, EventHandler> >::iterator itr = m_events.find(id);
    if (itr == m_events.end()) {
      return false;
    }

    std::multimap<float, EventHandler> &tab = itr->second;

    for (std::multimap<float, EventHandler>::iterator ev = tab.begin(); ev != tab.end(); ++ev) {
      if (ev->second == handler && (isnan(order) || ev->first == order)) {
        tab.erase(ev);

        if (tab.empty()) {
          m_events.erase(id);
        }

        EventDetached(id);
        return true;
      }
    }

    return false;
  }

  // TODO: fix all dis shit
  void Layout::LuaFrameEventHandler::Call(EventHandle *handle) const {
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fev");
    lua_rawgeti(L, -1, idx);
    lua_remove(L, -2);
    layout->l_push(L);
    lua_newtable(L);

    int parametercount = (*reinterpret_cast<typename LayoutHandlerMunger<void ()>::T>(pusher))(L);
    
    if (lua_pcall(L, parametercount + 2, 0, 0)) {
      // Error!
      layout->GetEnvironment()->LogError(lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }
  void Layout::LuaFrameEventHandler::Call(EventHandle *handle, int p1) const {
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fev");
    lua_rawgeti(L, -1, idx);
    lua_remove(L, -2);
    layout->l_push(L);
    lua_newtable(L);

    int parametercount = (*reinterpret_cast<typename LayoutHandlerMunger<void (int)>::T>(pusher))(L, p1);
    
    if (lua_pcall(L, parametercount + 2, 0, 0)) {
      // Error!
      layout->GetEnvironment()->LogError(lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }
  void Layout::LuaFrameEventHandler::Call(EventHandle *handle, const std::string &text) const {
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fev");
    lua_rawgeti(L, -1, idx);
    lua_remove(L, -2);
    layout->l_push(L);
    lua_newtable(L);

    int parametercount = (*reinterpret_cast<typename LayoutHandlerMunger<void (const std::string &)>::T>(pusher))(L, text);
    
    if (lua_pcall(L, parametercount + 2, 0, 0)) {
      // Error!
      layout->GetEnvironment()->LogError(lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }
  void Layout::LuaFrameEventHandler::Call(EventHandle *handle, const Point &pt) const {
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fev");
    lua_rawgeti(L, -1, idx);
    lua_remove(L, -2);
    layout->l_push(L);
    lua_newtable(L);

    int parametercount = (*reinterpret_cast<typename LayoutHandlerMunger<void (const Point &)>::T>(pusher))(L, pt);
    
    if (lua_pcall(L, parametercount + 2, 0, 0)) {
      // Error!
      layout->GetEnvironment()->LogError(lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }
  void Layout::LuaFrameEventHandler::Call(EventHandle *handle, const KeyEvent &ke) const {
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fev");
    lua_rawgeti(L, -1, idx);
    lua_remove(L, -2);
    layout->l_push(L);
    lua_newtable(L);

    int parametercount = (*reinterpret_cast<typename LayoutHandlerMunger<void (const KeyEvent &)>::T>(pusher))(L, ke);
    
    if (lua_pcall(L, parametercount + 2, 0, 0)) {
      // Error!
      layout->GetEnvironment()->LogError(lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }

  bool operator<(const Layout::LuaFrameEventHandler &lhs, const Layout::LuaFrameEventHandler &rhs) {
    if (lhs.idx != rhs.idx) return lhs.idx < rhs.idx;
    if (lhs.L != rhs.L) return lhs.L < rhs.L;
    return 0;
  }

  // for remove, I need to be able to look it up via event idx priority
  // actually really I need to have a refcounted point
  template<typename Prototype> void Layout::l_EventAttach(lua_State *L, EventId event, int idx, float priority, typename LayoutHandlerMunger<Prototype>::T pusher) {
    // Need to wrap up L and idx into a structure
    // Then insert that structure
    LuaFrameEventMap::iterator itr = m_lua_events.insert(std::make_pair(LuaFrameEventHandler(L, idx, this, reinterpret_cast<Utility::intfptr_t>(pusher)), 0)).first;
    ++(itr->second);
    EventAttach(event, EventHandler(Delegate<typename Utility::FunctionPrefix<EventHandle *, Prototype>::T>(&itr->first, &LuaFrameEventHandler::Call), &itr->first), priority);
  }

  template<typename Prototype> bool Layout::l_EventDetach(lua_State *L, EventId event, int idx, float priority) {
    // try to find the element
    LuaFrameEventMap::iterator itr = m_lua_events.find(LuaFrameEventHandler(L, idx, this, 0));  // fake pusher index, but that isn't compared anyway
    if (itr == m_lua_events.end()) {
      return false;
    }

    return l_EventDetach(L, itr, event, EventHandler(Delegate<typename Utility::FunctionPrefix<EventHandle *, Prototype>::T>(&itr->first, &LuaFrameEventHandler::Call), &itr->first), priority);
  }
  bool Layout::l_EventDetach(lua_State *L, LuaFrameEventMap::iterator itr, EventId event, EventHandler handler, float priority) {
    Environment::LuaStackChecker checker(L, GetEnvironment());

    if (EventDetach(event, handler, priority)) {
      if (--(itr->second) == 0) {
        // want to eliminate it entirely
        m_lua_events.erase(itr);
      }

      // Success! Go decrement the count
      // Right now we're assuming we don't have access to upvalues, but in reality we might. Maybe make this faster later?
      int idx = itr->first.idx;
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_cfev");
      lua_rawgeti(L, -1, idx);
      if (lua_tonumber(L, -1) != 1) {
        // stack: ... _cfev ct
        lua_pushnumber(L, lua_tonumber(L, -1) - 1);
        // stack: ... _cfev ct ct-1
        lua_rawseti(L, -3, idx);
        // stack: ... _cfev ct
        lua_pop(L, 2);
        // stack: ...
      } else {
        // our final decrement, clean things up
        lua_pop(L, 1);

        // stack: ... _cfev

        lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rfev");

        // grab the value from the frame event table
        lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fev");
        lua_rawgeti(L, -1, idx);

        // stack: ... _cfev _rfev _fev value

        // clear out the reverse event table
        lua_pushnil(L);
        // stack: ... _cfev _rfev _fev value nil
        lua_rawset(L, -4);

        // stack: ... _cfev _rfev _fev

        // clear out the forward event table
        luaL_unref(L, -1, idx);

        // clear out the frame count table
        lua_pushnil(L);
        lua_rawseti(L, -3, idx);

        // empty
        lua_pop(L, 3);

        // stack: ...
      }

      return true;
    } else {
      return false;
    }
  }

  /*static*/ int Layout::l_GetLeft(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetLeft());

    return 1;
  }

  /*static*/ int Layout::l_GetRight(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetRight());

    return 1;
  }

  /*static*/ int Layout::l_GetTop(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetTop());

    return 1;
  }

  /*static*/ int Layout::l_GetBottom(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetBottom());

    return 1;
  }

  /*static*/ int Layout::l_GetBounds(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetLeft());
    lua_pushnumber(L, self->GetTop());
    lua_pushnumber(L, self->GetRight());
    lua_pushnumber(L, self->GetBottom());

    return 4;
  }

  /*static*/ int Layout::l_GetWidth(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetWidth());

    return 1;
  }

  /*static*/ int Layout::l_GetHeight(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetHeight());

    return 1;
  }

  /*static*/ int Layout::l_GetChildren(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_newtable(L);

    const ChildrenList &children = self->GetChildren();
    for (ChildrenList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
      (*itr)->l_push(L);
      lua_rawseti(L, -2, lua_objlen(L, -2));
    }

    return 1;
  }

  /*static*/ int Layout::l_GetName(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushstring(L, self->GetName().c_str());

    return 1;
  }

  /*static*/ int Layout::l_GetNameFull(lua_State *L) {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushstring(L, self->GetNameFull().c_str());

    return 1;
  }

  /*static*/ int Layout::l_GetType(lua_State *L)  {
    l_checkparams(L, 1);
    Layout *self = l_checkframe<Layout>(L, 1);

    lua_pushstring(L, self->GetType());

    return 1;
  }
}

