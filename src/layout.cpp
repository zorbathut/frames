
#include "frames/layout.h"

#include "frames/environment.h"
#include "frames/event_definition.h"
#include "frames/lua.h"
#include "frames/rect.h"
#include "frames/renderer.h"

#include <math.h> // just for isnan()

namespace Frames {
  FRAMES_FRAMEEVENT_DEFINE(Move, ());
  FRAMES_FRAMEEVENT_DEFINE(Size, ());

  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseOver, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseMove, (const Point &pt));
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseMoveoutside, (const Point &pt));
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseOut, ());

  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseLeftUp, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseLeftUpoutside, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseLeftDown, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseLeftClick, ());

  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseMiddleUp, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseMiddleUpoutside, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseMiddleDown, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseMiddleClick, ());

  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseRightUp, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseRightUpoutside, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseRightDown, ());
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseRightClick, ());

  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseButtonUp, (int button));
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseButtonUpoutside, (int button));
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseButtonDown, (int button));
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseButtonClick, (int button));

  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(MouseWheel, (int delta));

  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(KeyDown, (const KeyEvent &kev));
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(KeyType, (const std::string &text));
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(KeyRepeat, (const KeyEvent &kev));
  FRAMES_FRAMEEVENT_DEFINE_BUBBLE(KeyUp, (const KeyEvent &kev));
  
  BOOST_STATIC_ASSERT(sizeof(EventId) == sizeof(intptr_t));
  BOOST_STATIC_ASSERT(sizeof(EventId) == sizeof(void *));

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
          axa.cached = std::numeric_limits<float>::quiet_NaN(); // this one really isn't a number, it's not just a sentinel value
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
          axb.cached = std::numeric_limits<float>::quiet_NaN(); // this one really isn't a number, it's not just a sentinel value
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

    // It's worth noting that we're guaranteed to validate our size while doing this, which means that anything deriving its info from us *will* be properly invalidated later if necessary

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
        ax.size_cached = std::numeric_limits<float>::quiet_NaN(); // this one really isn't a number, it's not just a sentinel value
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

  Layout *Layout::GetFrameUnder(float x, float y) {
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

  void Layout::luaF_push(lua_State *L) const {
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rrg");

    if (lua_isnil(L, -1)) {
      FRAMES_ERROR("Attempting to push frame into environment without Frame registered");
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
        FRAMES_ERROR("Frame environment not registered with this lua environment");

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
        Environment::LuaStackChecker lsc(L, m_env);
        luaF_Register(L);
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

    name = Utility::Format("[%s %s]", GetType(), name.c_str());

    return name;
  }

  std::string Layout::GetNameFull() const {
    std::string name;
    if (m_parent) {
      name = m_parent->GetNameFull() + ".";
    }

    return name + GetName();
  }

  void Layout::DebugDumpLayout() const {
    FRAMES_DEBUG("Dump for layout %s", GetNameFull().c_str());
    FRAMES_DEBUG("  XAXIS:");
    FRAMES_DEBUG("    Connector 0 from %f to %08x:%f offset %f, cache %f", m_axes[X].connections[0].point_mine, (int)m_axes[X].connections[0].link, m_axes[X].connections[0].point_link, m_axes[X].connections[0].offset, m_axes[X].connections[0].cached);
    FRAMES_DEBUG("    Connector 1 from %f to %08x:%f offset %f, cache %f", m_axes[X].connections[1].point_mine, (int)m_axes[X].connections[1].link, m_axes[X].connections[1].point_link, m_axes[X].connections[1].offset, m_axes[X].connections[0].cached);
    FRAMES_DEBUG("    Size %f (def %f), cache %f", m_axes[X].size_set, m_axes[X].size_default, m_axes[X].size_cached);
    FRAMES_DEBUG("    Linkcount %d", m_axes[X].children.size());
    FRAMES_DEBUG("  YAXIS:");
    FRAMES_DEBUG("    Connector 0 from %f to %08x:%f offset %f, cache %f", m_axes[Y].connections[0].point_mine, (int)m_axes[Y].connections[0].link, m_axes[Y].connections[0].point_link, m_axes[Y].connections[0].offset, m_axes[Y].connections[0].cached);
    FRAMES_DEBUG("    Connector 1 from %f to %08x:%f offset %f, cache %f", m_axes[Y].connections[1].point_mine, (int)m_axes[Y].connections[1].link, m_axes[Y].connections[1].point_link, m_axes[Y].connections[1].offset, m_axes[Y].connections[0].cached);
    FRAMES_DEBUG("    Size %f (def %f), cache %f", m_axes[Y].size_set, m_axes[Y].size_default, m_axes[Y].size_cached);
    FRAMES_DEBUG("    Linkcount %d", m_axes[Y].children.size());
  }

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
      m_alpha(1),
      m_fullMouseMasking(false),
      m_acceptInput(false),
      m_name_static(0),
      m_name_id(-1),
      m_obliterate_lock(0),
      m_obliterate_buffered(false),
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
    // TODO: come up with a better panic button for this? if we have no parent or environment then we have no way to do debug logging
    FRAMES_LAYOUT_CHECK(layout || env, "Layout not given parent or environment");

    m_constructionOrder = m_env->RegisterFrame();

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
    
    // Take out all our event handlers
    while (!m_events.empty()) {
      EventLookup::iterator eventTable = m_events.begin();
      
      while (!eventTable->second.empty()) {
        EventDestroy(eventTable, eventTable->second.begin()); // kaboom!
      }
    }

    // Clean up all appropriate lua environments
    for (std::set<lua_State *>::const_iterator itr = m_env->m_lua_environments.begin(); itr != m_env->m_lua_environments.end(); ++itr) {
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

  void Layout::SetPoint(Axis axis, float mypt, const Layout *link, float linkpt, float offset /*= 0.f*/) {
    if (link && link->m_env != m_env) {
      FRAMES_LAYOUT_CHECK(false, "Attempted to constrain a frame to a frame from another environment");
      return;
    }

    AxisData &ax = m_axes[axis];

    AxisData::Connector &axa = ax.connections[0];
    if (axa.point_mine == mypt) {
      // We don't care if we haven't changed
      if (axa.link == link && (!link || axa.point_link == linkpt) && axa.offset == offset) {
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
      if (axb.link == link && (!link || axb.point_link == linkpt) && axb.offset == offset) {
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
      FRAMES_LAYOUT_CHECK(link, "Frame overconstrained - attempted to assign a second point to a frame axis that already contained a size and one point.");
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

    if (link)
    {
      link->m_axes[axis].children.insert(this);
    }

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

    ClearSize(axis);
  }

  void Layout::ClearAllPoints() {
    ClearAllPoints(X);
    ClearAllPoints(Y);
  }

  // SetPoint adapters
  // All the anchor versions just transform themselves into no-anchor versions first
  void Layout::SetPoint(Anchor myanchor, const Layout *link, Anchor theiranchor) {
    SetPoint(c_anchorLookup[myanchor].x, c_anchorLookup[myanchor].y, link, c_anchorLookup[theiranchor].x, c_anchorLookup[theiranchor].y);
  }
  void Layout::SetPoint(Anchor myanchor, const Layout *link, Anchor theiranchor, float xofs, float yofs) {
    SetPoint(c_anchorLookup[myanchor].x, c_anchorLookup[myanchor].y, link, c_anchorLookup[theiranchor].x, c_anchorLookup[theiranchor].y, xofs, yofs);
  }
  
  void Layout::SetPoint(Anchor myanchor, const Layout *link, float theirx, float theiry) {
    SetPoint(c_anchorLookup[myanchor].x, c_anchorLookup[myanchor].y, link, theirx, theiry);
  }
  void Layout::SetPoint(Anchor myanchor, const Layout *link, float theirx, float theiry, float xofs, float yofs) {
    SetPoint(c_anchorLookup[myanchor].x, c_anchorLookup[myanchor].y, link, theirx, theiry, xofs, yofs);
  }

  void Layout::SetPoint(float myx, float myy, const Layout *link, Anchor theiranchor) {
    SetPoint(myx, myy, link, c_anchorLookup[theiranchor].x, c_anchorLookup[theiranchor].y);
  }
  void Layout::SetPoint(float myx, float myy, const Layout *link, Anchor theiranchor, float xofs, float yofs) {
    SetPoint(myx, myy, link, c_anchorLookup[theiranchor].x, c_anchorLookup[theiranchor].y, xofs, yofs);
  }

  void Layout::SetPoint(float myx, float myy, const Layout *link, float theirx, float theiry) {
    FRAMES_LAYOUT_CHECK(link, "SetPoint requires offsets when linking to origin.");
    FRAMES_LAYOUT_CHECK(IsNil(myx) == IsNil(theirx), "SetPoint provided with only one anchor position for X axis");
    FRAMES_LAYOUT_CHECK(IsNil(myy) == IsNil(theiry), "SetPoint provided with only one anchor position for Y axis");
    FRAMES_LAYOUT_CHECK(!IsNil(myx) || !IsNil(myy), "SetPoint not provided with any anchor axes");

    if (!IsNil(myx)) {
      SetPoint(X, myx, link, theirx);
    }

    if (!IsNil(myy)) {
      SetPoint(Y, myy, link, theiry);
    }
  }
  void Layout::SetPoint(float myx, float myy, const Layout *link, float theirx, float theiry, float xofs, float yofs) {
    FRAMES_LAYOUT_CHECK(!IsNil(myx) || !IsNil(myy), "SetPoint not provided with any anchor axes");
    if (link) {
      FRAMES_LAYOUT_CHECK(IsNil(myx) == IsNil(theirx) && IsNil(myx) == IsNil(xofs), "SetPoint provided with only one anchor position for X axis");
      FRAMES_LAYOUT_CHECK(IsNil(myy) == IsNil(theiry) && IsNil(myy) == IsNil(yofs), "SetPoint provided with only one anchor position for Y axis");
    } else {
      FRAMES_LAYOUT_CHECK(IsNil(theirx) && IsNil(theiry), "SetPoint must have nil target anchor points when linking to origin.");
      FRAMES_LAYOUT_CHECK(IsNil(myx) == IsNil(xofs), "SetPoint provided with only one anchor position for X axis");
      FRAMES_LAYOUT_CHECK(IsNil(myy) == IsNil(yofs), "SetPoint provided with only one anchor position for Y axis");
    }

    if (!IsNil(myx)) {
      SetPoint(X, myx, link, theirx, xofs);
    }

    if (!IsNil(myy)) {
      SetPoint(Y, myy, link, theiry, yofs);
    }
  }

  void Layout::SetSize(Axis axis, float size) {
    AxisData &ax = m_axes[axis];

    if (!Utility::IsUndefined(ax.connections[0].point_mine) && !Utility::IsUndefined(ax.connections[1].point_mine)) {
      FRAMES_LAYOUT_CHECK(false, "Frame overconstrained - attempted to assign a size to a frame axis that already contained two points.");
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

  void Layout::ClearConstraints() {
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

    // We take ourselves out of our parents' list so we can change our layer without breaking the sort
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

    // We take ourselves out of our parents' list so we can change our strata without breaking the sort
    if (m_parent) {
      m_parent->m_children.erase(this);
    }

    m_strata = strata;

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
    if (m_obliterate_lock) {
      // can't do this quite yet, do it later
      m_obliterate_buffered = true;
      return;
    }

    Obliterate_Detach();
    Obliterate_Extract();
  }

  bool Layout::EventHookedIs(const EventTypeBase &event) const {
    std::map<const EventTypeBase *, std::multiset<FECallback, FECallback::Sorter> >::const_iterator itr = m_events.find(&event);
    if (itr == m_events.end()) {
      // no handles, we're good
      return false;
    }
    
    // We now need to iterate over all events just in case they're all destroy-flagged
    const std::multiset<FECallback, FECallback::Sorter> &eventSet = itr->second;
    for (std::multiset<FECallback, FECallback::Sorter>::const_iterator itr = eventSet.begin(); itr != eventSet.end(); ++itr) {
      if (!itr->DestroyFlagGet()) {
        return true;
      }
    }
    
    // they are! sigh
    return false;
  }

  void Layout::EventAttached(const EventTypeBase *event) {
    if (!m_acceptInput) {
      m_acceptInput =
        EventHookedIs(Event::MouseLeftClick) || EventHookedIs(Event::MouseLeftUp) || EventHookedIs(Event::MouseLeftDown) ||
        EventHookedIs(Event::MouseMiddleClick) || EventHookedIs(Event::MouseMiddleUp) || EventHookedIs(Event::MouseMiddleDown) ||
        EventHookedIs(Event::MouseRightClick) || EventHookedIs(Event::MouseRightUp) || EventHookedIs(Event::MouseRightDown) ||
        EventHookedIs(Event::MouseButtonClick) || EventHookedIs(Event::MouseButtonUp) || EventHookedIs(Event::MouseButtonDown) ||
        EventHookedIs(Event::MouseMove) || EventHookedIs(Event::MouseWheel) || EventHookedIs(Event::MouseOver) || EventHookedIs(Event::MouseOut);
    }
  }

  void Layout::EventDetached(const EventTypeBase *event) {
    if (m_acceptInput) {
      m_acceptInput =
        EventHookedIs(Event::MouseLeftClick) || EventHookedIs(Event::MouseLeftUp) || EventHookedIs(Event::MouseLeftDown) ||
        EventHookedIs(Event::MouseMiddleClick) || EventHookedIs(Event::MouseMiddleUp) || EventHookedIs(Event::MouseMiddleDown) ||
        EventHookedIs(Event::MouseRightClick) || EventHookedIs(Event::MouseRightUp) || EventHookedIs(Event::MouseRightDown) ||
        EventHookedIs(Event::MouseButtonClick) || EventHookedIs(Event::MouseButtonUp) || EventHookedIs(Event::MouseButtonDown) ||
        EventHookedIs(Event::MouseMove) || EventHookedIs(Event::MouseWheel) || EventHookedIs(Event::MouseOver) || EventHookedIs(Event::MouseOut);
    }
  }

  void Layout::luaF_RegisterWorker(lua_State *L, const char *name) const {
    Environment::LuaStackChecker lsc(L, m_env);
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

  /*static*/ void Layout::luaF_RegisterFunctions(lua_State *L) {
    luaF_RegisterFunction(L, GetStaticType(), "GetLeft", luaF_GetLeft);
    luaF_RegisterFunction(L, GetStaticType(), "GetRight", luaF_GetRight);
    luaF_RegisterFunction(L, GetStaticType(), "GetTop", luaF_GetTop);
    luaF_RegisterFunction(L, GetStaticType(), "GetBottom", luaF_GetBottom);
    luaF_RegisterFunction(L, GetStaticType(), "GetBounds", luaF_GetBounds);

    luaF_RegisterFunction(L, GetStaticType(), "GetWidth", luaF_GetWidth);
    luaF_RegisterFunction(L, GetStaticType(), "GetHeight", luaF_GetHeight);

    luaF_RegisterFunction(L, GetStaticType(), "GetChildren", luaF_GetChildren);

    luaF_RegisterFunction(L, GetStaticType(), "GetName", luaF_GetName);
    luaF_RegisterFunction(L, GetStaticType(), "GetNameFull", luaF_GetNameFull);
    luaF_RegisterFunction(L, GetStaticType(), "GetType", luaF_GetType);
    
    luaF_RegisterFunction(L, GetStaticType(), "EventAttach", luaF_EventAttach);
    luaF_RegisterFunction(L, GetStaticType(), "EventDetach", luaF_EventDetach);

    luaF_RegisterFunction(L, GetStaticType(), "DebugDumpLayout", luaF_DebugDumpLayout);
  }

  /*static*/ void Layout::luaF_RegisterFunction(lua_State *L, const char *owner, const char *name, int (*func)(lua_State *)) {
    // From Environment::RegisterLuaFrame
    // Stack: ... Frames_mt Frames_rg Frames_fevh Frames_rfevh Frames_cfevh metatable indexes
    lua_getfield(L, -6, owner);
    lua_pushcclosure(L, func, 1);
    lua_setfield(L, -2, name);
  }

  void Layout::Render(Renderer *renderer) const {
    if (m_visible) {
      renderer->AlphaPush(GetAlpha());
      RenderElement(renderer);

      for (ChildrenList::const_iterator itr = m_children.begin(); itr != m_children.end(); ++itr) {
        (*itr)->Render(renderer);
      }
      
      RenderElementPost(renderer);
      renderer->AlphaPop();
    }
  }

  // Technically, invalidate could be a lot more specific. There are cases where invalidating an entire axis is unnecessary - A depends on one connection, B depends on another connection, and only A's connection is invalidated.
  // It's unclear if this would be worth the additional overhead and complexity.
  // Invalidating extra stuff, while slow, is at least always correct.
  void Layout::Invalidate(Axis axis) {
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
    ClearConstraints();  // kill my layout to unlink things

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
      ChildrenList::const_iterator kill = itr;
      ++itr;
      (*kill)->Obliterate_Extract();
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

  void Layout::Resolve() {
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
      EventTrigger(Event::Size);
    }

    if (sizechange || movechange) {
      EventTrigger(Event::Move);
    }

    // Todo: queue up movement events
  }

  bool Layout::FECallback::Sorter::operator()(const Layout::FECallback &lhs, const Layout::FECallback &rhs) const {
    return lhs.m_priority < rhs.m_priority;
  }
  
  void Layout::FECallback::Teardown(Environment *env) const {
    if (m_type == TYPE_LUA) {
      // Derefcount, cleanup
      lua_State *L = c.lua.L;
      Environment::LuaStackChecker lsc(L, env);
      
      // stack: ...
      
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_cfevh");
      lua_rawgeti(L, -1, c.lua.handle);
      if (lua_tointeger(L, -1) > 1) {
        // don't dealloc, just update
        lua_pushinteger(L, lua_tointeger(L, -1) - 1);
        lua_remove(L, -2);
        lua_rawseti(L, -2, c.lua.handle);
        lua_pop(L, 2);
        // cleaned up!
      } else {
        // need a full dealloc
        lua_pop(L, 1);
        lua_pushnil(L);
        lua_rawseti(L, -2, c.lua.handle);
        lua_pop(L, 1);
        // cleaned up, but still working
        
        // we need to clean up the forward lookup next, but keep a handle around so we can clean up the reverse lookup
        lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fevh");
        lua_rawgeti(L, -1, c.lua.handle);
        // ... Frames_fevh func
        luaL_unref(L, -2, c.lua.handle);
        lua_remove(L, -2);
        // ... func
        
        lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rfevh");
        lua_insert(L, -2);
        lua_pushnil(L);
        // ... Frames_rfevh func nil
        lua_rawset(L, -3);
        lua_pop(L, 1);
        // cleaned up! really!
      }
    }
  }
  
  int Layout::FECallback::luaF_prepare(EventHandle *eh) const {
    lua_State *L = c.lua.L;
    
    int ctop = lua_gettop(L);
    
    // error handler
    eh->GetTarget()->GetEnvironment()->Lua_PushErrorHandler(L);
    
    // get function
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fevh");
    lua_rawgeti(L, -1, c.lua.handle);
    lua_remove(L, -2);
    
    // target frame
    eh->GetTarget()->luaF_push(L);
    
    // event handle
    eh->luaF_push(L);
    
    return ctop;
  }
  
  void Layout::FECallback::luaF_call(int stackfront) const {
    // TODO: Better error handling!
    lua_State *L = c.lua.L;
    if (lua_pcall(L, lua_gettop(c.lua.L) - stackfront - 2, 0, stackfront + 1)) {
      // Error occured. TODO detect LUA_ERRERR and complain about it?
      lua_pop(L, 1);
    }
  }
      
  Layout::FEIterator::FEIterator() : m_state(STATE_COMPLETE), m_diveIndex(0), m_target(0), m_event(0) { };

  Layout::FEIterator::FEIterator(Layout *target, const EventTypeBase *event) : m_state(STATE_DIVE), m_diveIndex(0), m_target(target), m_event(event) { // set to STATE_DIVE so that NextIndex() does the right thing
    target->Obliterate_Lock();
    
    if (event->GetDive()) {
      // Dive event! Accumulate everything we need
      Layout *ctar = target;
      while (ctar) {
        m_dives.push_back(ctar);
        ctar->Obliterate_Lock();
        ctar = ctar->GetParent();
      }
      m_diveIndex = m_dives.size();
      
      IndexNext();
    } else {
      IndexNext();
    }
    
    if (m_state != STATE_COMPLETE) {
      m_current->LockFlagIncrement();
    }
  };
  
  Layout::FEIterator::FEIterator(const FEIterator &itr) : m_state(STATE_COMPLETE), m_diveIndex(0), m_target(0), m_event(0) {
    *this = itr;
  }
  
  void Layout::FEIterator::operator=(const FEIterator &itr) {
    if (this == &itr) return;
    
    if (m_state != STATE_COMPLETE) {
      IteratorUnlock(m_current);
    }
    
    if (m_target) {
      m_target->Obliterate_Unlock();
    }
    
    for (int i = 0; i < (int)m_dives.size(); ++i) {
      m_dives[i]->Obliterate_Unlock();
    }
    
    m_state = itr.m_state;
    m_dives = itr.m_dives;
    m_diveIndex = itr.m_diveIndex;
    m_target = itr.m_target;
    m_event = itr.m_event;
    m_current = itr.m_current;
    m_last = itr.m_last;
    
    for (int i = 0; i < (int)m_dives.size(); ++i) {
      m_dives[i]->Obliterate_Lock();
    }
    
    if (m_target) {
      m_target->Obliterate_Lock();
    }
    
    if (m_state != STATE_COMPLETE) {
      m_current->LockFlagIncrement();
    }
  }
  
  Layout::FEIterator::~FEIterator() {
    if (m_state != STATE_COMPLETE) {
      IteratorUnlock(m_current);
    }
    
    if (m_target) {
      m_target->Obliterate_Unlock();
    }
  }
  
  const Layout::FECallback &Layout::FEIterator::Get() const {
    return *m_current;
  }
  
  bool Layout::FEIterator::Complete() const {
    return m_state == STATE_COMPLETE;
  }
  
  void Layout::FEIterator::Next() {
    EventMultiset::iterator last = m_current;
    ++m_current;
    
    if (m_current == m_last) {
      IndexNext();
    }
    
    if (m_state != STATE_COMPLETE) {
      m_current->LockFlagIncrement();
    }
    
    IteratorUnlock(last);
  }
  
  void Layout::FEIterator::IteratorUnlock(EventMultiset::iterator itr) {
    itr->LockFlagDecrement();
    if (!itr->LockFlagGet() && itr->DestroyFlagGet()) {
      // TIME TO DESTROY
      Layout *layout = m_target;
      if (m_state == STATE_DIVE || m_state == STATE_BUBBLE) {
        layout = m_dives[m_diveIndex];
      }
      
      const EventTypeBase *event = m_event;
      if (m_state == STATE_DIVE) {
        event = event->GetDive();
      } else if (m_state == STATE_BUBBLE) {
        event = event->GetBubble();
      }
      
      layout->EventDestroy(layout->m_events.find(event), itr);
    }
  }
  
  void Layout::FEIterator::IndexNext() {
    while (true) {
      if (m_state == STATE_DIVE) {
        if (m_diveIndex == 0) {
          m_state = STATE_MAIN;
        } else {
          --m_diveIndex;
        }
      } else if (m_state == STATE_MAIN) {
        if (m_dives.empty()) {
          m_state = STATE_COMPLETE;
          return;
        } else {
          m_state = STATE_BUBBLE; // m_diveIndex is still 0
        }
      } else if (m_state == STATE_BUBBLE) {
        if (m_diveIndex == (int)m_dives.size() - 1) {
          m_state = STATE_COMPLETE;
          return;
        } else {
          ++m_diveIndex;
        }
      }
      
      Layout *layout = LayoutGet();
      const EventTypeBase *event = EventGet();
      
      // TODO: can probably be optimized by not doing a ton of lookups
      if (layout->m_events.count(event)) {
        // Sweet! We actually have events here
        m_current = layout->m_events[event].begin();
        m_last = layout->m_events[event].end();
        return;
      }
    }
  }
  
  Layout *Layout::FEIterator::LayoutGet() {
    if (m_state == STATE_DIVE || m_state == STATE_BUBBLE) {
      return m_dives[m_diveIndex];
    } else {
      return m_target;
    }
  }
  
  const EventTypeBase *Layout::FEIterator::EventGet() {
    if (m_state == STATE_DIVE) {
      return m_event->GetDive();
    } else if (m_state == STATE_BUBBLE) {
      return m_event->GetBubble();
    } else {
      return m_event;
    }
  }
  
  bool Layout::FrameOrderSorter::operator()(const Layout *lhs, const Layout *rhs) const {
    if (lhs->GetStrata() != rhs->GetStrata())
      return lhs->GetStrata() < rhs->GetStrata();
    if (lhs->GetLayer() != rhs->GetLayer())
      return lhs->GetLayer() < rhs->GetLayer();
    // they're the same, but we want a consistent sort that won't result in Z-fighting
    return lhs->m_constructionOrder < rhs->m_constructionOrder;
  }
  
  void Layout::EventDestroy(EventLookup::iterator eventTable, EventMultiset::iterator toBeRemoved) {
    if (toBeRemoved->LockFlagGet()) {
      toBeRemoved->DestroyFlagSet();
    } else {
      toBeRemoved->Teardown(m_env);
      eventTable->second.erase(toBeRemoved);
      if (eventTable->second.empty()) {
        // we know nobody's got it locked, so . . .
        m_events.erase(eventTable);
      }
    }
  }
  
  void Layout::luaF_ClearEvents_Recursive(lua_State *L) {
    // Take out all our event handlers
    for (EventLookup::iterator itr = m_events.begin(); itr != m_events.end(); ) {
      EventLookup::iterator next = itr;
      ++next;
      
      for (EventMultiset::iterator eitr = itr->second.begin(); eitr != itr->second.end(); ) {
        EventMultiset::iterator enext = eitr;
        ++enext;
        
        bool nextend = (enext == itr->second.end());
        
        if (eitr->LuaIs() && eitr->LuaEnvironmentEqual(L)) {
          EventDestroy(itr, eitr);
        }
        
        eitr = enext;
        
        // The entire event multiset may have vanished out from underneath us.
        // TODO maybe adapt the existing event iterators to make this less horrifying?
        if (nextend) {
          break;
        }
      }
      
      itr = next;
    }
    
    // get children to clear events as well
    for (ChildrenList::iterator itr = m_children.begin(); itr != m_children.end(); ++itr) {
      (*itr)->luaF_ClearEvents_Recursive(L);
    }
  }

  void Layout::Obliterate_Lock() {
    ++m_obliterate_lock;
  }

  void Layout::Obliterate_Unlock() {
    FRAMES_LAYOUT_CHECK(m_obliterate_lock, "Unlocking frame obliterate when already unlocked, internal error");
    --m_obliterate_lock;
    if (!m_obliterate_lock && m_obliterate_buffered) {
      Obliterate(); // kaboom!
    }
  }

  /*static*/ int Layout::luaF_GetLeft(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetLeft());

    return 1;
  }

  /*static*/ int Layout::luaF_GetRight(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetRight());

    return 1;
  }

  /*static*/ int Layout::luaF_GetTop(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetTop());

    return 1;
  }

  /*static*/ int Layout::luaF_GetBottom(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetBottom());

    return 1;
  }

  /*static*/ int Layout::luaF_GetBounds(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetLeft());
    lua_pushnumber(L, self->GetTop());
    lua_pushnumber(L, self->GetRight());
    lua_pushnumber(L, self->GetBottom());

    return 4;
  }

  /*static*/ int Layout::luaF_GetWidth(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetWidth());

    return 1;
  }

  /*static*/ int Layout::luaF_GetHeight(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushnumber(L, self->GetHeight());

    return 1;
  }

  /*static*/ int Layout::luaF_GetChildren(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_newtable(L);

    const ChildrenList &children = self->GetChildren();
    for (ChildrenList::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
      (*itr)->luaF_push(L);
      lua_rawseti(L, -2, lua_objlen(L, -2));
    }

    return 1;
  }

  /*static*/ int Layout::luaF_GetName(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushstring(L, self->GetName().c_str());

    return 1;
  }

  /*static*/ int Layout::luaF_GetNameFull(lua_State *L) {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushstring(L, self->GetNameFull().c_str());

    return 1;
  }

  /*static*/ int Layout::luaF_GetType(lua_State *L)  {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    lua_pushstring(L, self->GetType());

    return 1;
  }

  /*static*/ int Layout::luaF_EventAttach(lua_State *L) {
    luaF_checkparams(L, 3, 4);
    
    // Stack: layout eventhandle handler (priority)
    
    Layout *self = luaF_checkframe<Layout>(L, 1);
    const EventTypeBase *event = luaF_checkevent(L, 2);
    float priority = (float)luaL_optnumber(L, 4, 0.f);
    
    int handler = LUA_NOREF;
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rfevh");
    lua_pushvalue(L, 3);
    lua_rawget(L, -2);
    if (!lua_isnil(L, -1)) {
      // Found element, need to retrieve ID and increment refcount
      // Stack: ... _rfevh handleid
      handler = lua_tointeger(L, -1);
      lua_pop(L, 2);
      
      // Stack: ...
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_cfevh");
      lua_rawgeti(L, -1, handler);
      lua_pushinteger(L, lua_tointeger(L, -1) + 1);
      lua_remove(L, -2);
      lua_rawseti(L, -2, handler);
      lua_pop(L, 1);
      
      // Stack: ...
    } else {
      // Didn't find element, need to insert element
      // Set _fevh lookup
      // Stack: ... _rfevh nil
      lua_pop(L, 1);
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fevh");
      // Stack: ... _rfevh _fevh
      lua_pushvalue(L, 3);
      // Stack: ... _rfevh _fevh function
      handler = luaL_ref(L, -2);
      lua_pop(L, 1);
      // Stack: ... _rfevh
      
      // Set _rfevh lookup
      lua_pushvalue(L, 3);
      lua_pushinteger(L, handler);
      lua_rawset(L, -3);
      lua_pop(L, 1);
      // Stack: ...
      
      // Insert into _cfevh
      lua_getfield(L, LUA_REGISTRYINDEX, "Frames_cfevh");
      lua_pushinteger(L, 1);
      lua_rawseti(L, -2, handler);
      lua_pop(L, 1);
      // Stack: ...
    }
    
    // now we have all valid parameters!
    // get the base lua environment
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_lua");
    lua_State *L_base = (lua_State *)lua_touserdata(L, -1);
    lua_pop(L, 1);
  
    self->m_events[event].insert(FECallback::CreateLua(L_base, handler, priority));
    self->EventAttached(event);
   
    return 0;
  }
  
  /*static*/ int Layout::luaF_EventDetach(lua_State *L) {
    luaF_checkparams(L, 3, 4);
    
    // Stack: layout eventhandle handler (priority)
    
    Layout *self = luaF_checkframe<Layout>(L, 1);
    const EventTypeBase *event = luaF_checkevent(L, 2);
    float priority = (float)luaL_optnumber(L, 4, Utility::Undefined);
    
    int handler = LUA_NOREF;
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rfevh");
    lua_pushvalue(L, 3);
    lua_rawget(L, -2);
    if (lua_isnil(L, -1)) {
      // well okay I guess we're done (not bothering to clean up stack)
      return 0;
    }
    
    lua_tointeger(L, -1);
    lua_pop(L, 2);
    
    if (!self->m_events.count(event)) {
      return 0;
    }
    
    // get the base lua environment
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_lua");
    lua_State *L_base = (lua_State *)lua_touserdata(L, -1);
    lua_pop(L, 1);
    
    const std::multiset<FECallback, FECallback::Sorter> &eventSet = self->m_events.find(event)->second;
    
    // TODO: Make this faster if it ever becomes a bottleneck!
    for (std::multiset<FECallback, FECallback::Sorter>::iterator itr = eventSet.begin(); itr != eventSet.end(); ++itr) {
      if (!itr->DestroyFlagGet() && itr->LuaHandleEqual(L_base, handler) && (Utility::IsUndefined(priority) || itr->PriorityGet() == priority)) {
        self->EventDestroy(self->m_events.find(event), itr);
        self->EventDetached(event);
        return 0;
      }
    }
    
    return 0;
  }
    
  /*static*/ int Layout::luaF_DebugDumpLayout(lua_State *L)  {
    luaF_checkparams(L, 1);
    Layout *self = luaF_checkframe<Layout>(L, 1);

    self->DebugDumpLayout();

    return 0;
  }
}

