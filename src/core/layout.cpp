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

#include "frames/layout.h"

#include "frames/cast.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
#include "frames/event_definition.h"
#include "frames/frame.h"
#include "frames/rect.h"
#include "frames/renderer.h"

#include <math.h> // just for isnan()

#include <limits>

namespace Frames {
  FRAMES_VERB_DEFINE(Layout::Event::Move, ());
  FRAMES_VERB_DEFINE(Layout::Event::Size, ());

  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseOver, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseMove, (const Vector &pt));
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseMoveoutside, (const Vector &pt));
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseOut, ());

  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseLeftUp, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseLeftUpoutside, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseLeftDown, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseLeftClick, ());

  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseMiddleUp, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseMiddleUpoutside, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseMiddleDown, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseMiddleClick, ());

  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseRightUp, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseRightUpoutside, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseRightDown, ());
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseRightClick, ());

  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseButtonUp, (int button));
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseButtonUpoutside, (int button));
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseButtonDown, (int button));
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseButtonClick, (int button));

  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::MouseWheel, (int delta));

  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::KeyDown, (Input::Key kev));
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::KeyText, (const std::string &text));
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::KeyRepeat, (Input::Key kev));
  FRAMES_VERB_DEFINE_BUBBLE(Layout::Event::KeyUp, (Input::Key kev));

  FRAMES_VERB_DEFINE(Layout::Event::Destroy, ());

  detail::Rtti Layout::s_rtti(0); // special case, null terminator; other examples should use FRAMES_DEFINE_RTTI
  /*static*/ const char *Layout::TypeStaticGet() {
    return "Layout";
  }

  Layout::PinAxis Layout::PinGet(Axis axis, float mypt) const {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return PinAxis();
    }

    const AxisData &ax = m_axes[axis];
    for (int i = 0; i < 2; ++i) {
      if (ax.connections[i].point_mine == mypt) {
        return PinAxis(ax.connections[i].target, ax.connections[i].point_target, ax.connections[i].offset);
      }
    }

    return PinAxis();
  }

  Layout::PinPoint Layout::PinGet(Anchor anchor) const {
    if (anchor < 0 || anchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return PinPoint();
    }

    return PinGet(detail::AnchorLookup[anchor].x, detail::AnchorLookup[anchor].y);
  }

  Layout::PinPoint Layout::PinGet(float x, float y) const {
    PinAxis pinX, pinY;
    if (!detail::IsNil(x)) {
      pinX = PinGet(X, x);
      if (!pinX.valid) {
        return PinPoint();
      }
    }
    if (!detail::IsNil(y)) {
      pinY = PinGet(Y, y);
      if (!pinY.valid) {
        return PinPoint();
      }
    }

    if (!detail::IsNil(x) && !detail::IsNil(y) && pinX.target != pinY.target) {
      return PinPoint();
    }

    return PinPoint(!detail::IsNil(x) ? pinX.target : pinY.target, Vector(pinX.point, pinY.point), Vector(pinX.offset, pinY.offset));
  }

  float Layout::PointGet(Axis axis, float pt) const {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return 0.f;
    }

    const AxisData &ax = m_axes[axis];

    // Check our caches
    const AxisData::Connector &axa = ax.connections[0];
    if (axa.point_mine == pt) {
      if (!detail::IsUndefined(axa.cached)) {
        if (detail::IsProcessing(axa.cached)) {
          m_env->LayoutStack_Push(this, axis, pt);
          m_env->LayoutStack_Error();
          m_env->LayoutStack_Pop();
          axa.cached = std::numeric_limits<float>::quiet_NaN(); // this one really isn't a number, it's not just a sentinel value
        }
        return axa.cached;
      }
      if (axa.target) {
        m_env->LayoutStack_Push(this, axis, pt);
        axa.cached = detail::Processing; // seed it with processing so we'll exit if this turns out to be an infinite loop
        axa.cached = axa.target->PointGet(axis, axa.point_target) + axa.offset;
        m_env->LayoutStack_Pop();
        return axa.cached;
      }
      axa.cached = axa.offset;
      return axa.cached;
    }

    const AxisData::Connector &axb = ax.connections[1];
    if (axb.point_mine == pt) {
      if (!detail::IsUndefined(axb.cached)) {
        if (detail::IsProcessing(axa.cached)) {
          m_env->LayoutStack_Push(this, axis, pt);
          m_env->LayoutStack_Error();
          m_env->LayoutStack_Pop();
          axb.cached = std::numeric_limits<float>::quiet_NaN(); // this one really isn't a number, it's not just a sentinel value
        }
        return axb.cached;
      }
      if (axb.target) {
        m_env->LayoutStack_Push(this, axis, pt);
        axb.cached = detail::Processing; // seed it with processing so we'll exit if this turns out to be an infinite loop
        axb.cached = axb.target->PointGet(axis, axb.point_target) + axb.offset;
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
    // In all cases, we assume size exists via SizeGet() - the only question is whether we have a vertex or not

    // It's worth noting that we're guaranteed to validate our size while doing this, which means that anything deriving its info from us *will* be properly invalidated later if necessary

    // Find a valid vertex - we'll only be using one
    const AxisData::Connector *connect;
    if (detail::IsUndefined(axa.point_mine)) {
      connect = &axb;
    } else {
      connect = &axa;
    }

    // 0 vertices, size
    if (detail::IsUndefined(connect->point_mine)) {
      m_env->LayoutStack_Push(this, axis, pt);
      float rv = pt * SizeGet(axis);
      m_env->LayoutStack_Pop();
      return rv;
    }

    // 1 vertex, size
    m_env->LayoutStack_Push(this, axis, pt);
    float rv = PointGet(axis, connect->point_mine) + (pt - connect->point_mine) * SizeGet(axis);
    m_env->LayoutStack_Pop();
    return rv;
  }

  Vector Layout::PointGet(Anchor anchor) const {
    if (anchor < 0 || anchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return Vector();
    }

    return PointGet(detail::AnchorLookup[anchor].x, detail::AnchorLookup[anchor].y);
  }

  Vector Layout::PointGet(const Vector &position) const {
    return PointGet(position.x, position.y);
  }

  Vector Layout::PointGet(float x, float y) const {
    return Vector(PointGet(X, x), PointGet(Y, y));
  }

  Rect Layout::BoundsGet() const {
    return Rect(LeftGet(), TopGet(), RightGet(), BottomGet());
  }

  float Layout::SizeGet(Axis axis) const {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return 0.f;
    }

    const AxisData &ax = m_axes[axis];

    // Check our cache
    if (!detail::IsUndefined(ax.size_cached)) {
      if (detail::IsProcessing(ax.size_cached)) {
        m_env->LayoutStack_Push(this, axis);
        m_env->LayoutStack_Error();
        m_env->LayoutStack_Pop();
        ax.size_cached = std::numeric_limits<float>::quiet_NaN(); // this one really isn't a number, it's not just a sentinel value
      }
      return ax.size_cached;
    }

    // Check an explicit setting
    if (!detail::IsUndefined(ax.size_set)) {
      ax.size_cached = ax.size_set;
      return ax.size_set;
    }

    // Let's see if we have two known points
    const AxisData::Connector &axa = ax.connections[0];
    const AxisData::Connector &axb = ax.connections[1];
    if (!detail::IsUndefined(axa.point_mine) && !detail::IsUndefined(axb.point_mine)) {
      // Woo! We can calculate from here
      m_env->LayoutStack_Push(this, axis);
      ax.size_cached = detail::Processing; // seed it with processing so we'll exit if this turns out to be an infinite loop
      float a = PointGet(axis, axa.point_mine);
      float b = PointGet(axis, axb.point_mine);
      m_env->LayoutStack_Pop();

      ax.size_cached = (a - b) / (axa.point_mine - axb.point_mine);
      return ax.size_cached;
    }

    // Default size it is
    ax.size_cached = ax.size_default;
    return ax.size_default;
  }

  Layout *Layout::ProbeAsMouse(float x, float y) const {
    if (!VisibleGet()) return 0; // nope

    if (m_fullMouseMasking && !MouseMaskingTest(x, y)) return 0;

    for (ChildrenList::const_reverse_iterator itr = m_children.rbegin(); itr != m_children.rend(); ++itr) {
      Layout *prv = (*itr)->ProbeAsMouse(x, y);
      if (prv) return prv;
    }

    if (m_inputMode && x >= LeftGet() && y >= TopGet() && x < RightGet() && y < BottomGet()) {
      return const_cast<Layout *>(this);  // this is sort of nasty, but it doesn't violate the spec and does conform to people's expectations
    }

    return 0;
  }

  Frame *Layout::ChildGetByName(const std::string &name) const {
    for (ChildrenList::const_iterator itr = m_children_nonimplementation.begin(); itr != m_children_nonimplementation.end(); ++itr) {
      if ((*itr)->NameGet() == name) {
        return *itr;
      }
    }

    return 0;
  }

  Frame *Layout::ChildImplementationGetByName(const std::string &name) const {
    for (ChildrenList::const_iterator itr = m_children_implementation.begin(); itr != m_children_implementation.end(); ++itr) {
      if ((*itr)->NameGet() == name) {
        return *itr;
      }
    }

    return 0;
  }

  void Layout::DebugLayoutDump() const {
    FRAMES_DEBUG("Dump for layout %s", DebugNameGet().c_str());
    FRAMES_DEBUG("  XAXIS:");
    FRAMES_DEBUG("    Connector 0 from %f to %08x:%f offset %f, cache %f", m_axes[X].connections[0].point_mine, (int)m_axes[X].connections[0].target, m_axes[X].connections[0].point_target, m_axes[X].connections[0].offset, m_axes[X].connections[0].cached);
    FRAMES_DEBUG("    Connector 1 from %f to %08x:%f offset %f, cache %f", m_axes[X].connections[1].point_mine, (int)m_axes[X].connections[1].target, m_axes[X].connections[1].point_target, m_axes[X].connections[1].offset, m_axes[X].connections[0].cached);
    FRAMES_DEBUG("    Size %f (def %f), cache %f", m_axes[X].size_set, m_axes[X].size_default, m_axes[X].size_cached);
    FRAMES_DEBUG("    Pincount %d", m_axes[X].children.size());
    FRAMES_DEBUG("  YAXIS:");
    FRAMES_DEBUG("    Connector 0 from %f to %08x:%f offset %f, cache %f", m_axes[Y].connections[0].point_mine, (int)m_axes[Y].connections[0].target, m_axes[Y].connections[0].point_target, m_axes[Y].connections[0].offset, m_axes[Y].connections[0].cached);
    FRAMES_DEBUG("    Connector 1 from %f to %08x:%f offset %f, cache %f", m_axes[Y].connections[1].point_mine, (int)m_axes[Y].connections[1].target, m_axes[Y].connections[1].point_target, m_axes[Y].connections[1].offset, m_axes[Y].connections[0].cached);
    FRAMES_DEBUG("    Size %f (def %f), cache %f", m_axes[Y].size_set, m_axes[Y].size_default, m_axes[Y].size_cached);
    FRAMES_DEBUG("    Pincount %d", m_axes[Y].children.size());
  }

  std::string Layout::DebugNameGet() const {
    std::string name;
    if (m_parent) {
      name = m_parent->DebugNameGet() + ".";
    }

    return name + NameGet();
  }

  // DUPLICATE CODE WARNING: Initializers are also used in the parent constructor!
  Layout::Layout(Environment *env, const std::string &name) :
      m_resolved(false),
      m_last_width(-1),
      m_last_height(-1),
      m_last_x(-1),
      m_last_y(-1),
      m_layer(0),
      m_implementation(false),
      m_parent(0),
      m_visible(true),
      m_fullMouseMasking(false),
      m_inputMode(IM_NONE),
      m_name(name),
      m_env(0)
  {
    if (!env) {
      Configuration::Get().LoggerGet()->LogError("Attempted to create layout without valid environment");
      return; // The chance of this not crashing is basically zero, but what can you do.
    }

    m_env = env;

    m_constructionOrder = m_env->RegisterFrame();

    m_env->MarkInvalidated(this); // Need to initialize things properly
  }

  Layout::~Layout() {
    // We shouldn't reach this point until all references to us have vanished, so, kapow!
    FRAMES_LAYOUT_CHECK(detail::IsUndefined(m_axes[X].connections[0].point_mine), "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(detail::IsUndefined(m_axes[X].connections[1].point_mine), "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(detail::IsUndefined(m_axes[Y].connections[0].point_mine), "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(detail::IsUndefined(m_axes[Y].connections[1].point_mine), "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(!m_parent, "Layout destroyed while still connected");
    FRAMES_LAYOUT_CHECK(m_children.empty(), "Layout destroyed while still connected");

    // Clear ourselves out of the resolved todo
    if (!m_resolved) {
      m_env->UnmarkInvalidated(this);
    }
    
    // Take out all our event handlers
    while (!m_events.empty()) {
      EventLookup::iterator eventTable = m_events.begin();
      EventDestroy(eventTable, eventTable->second.begin()); // kaboom!
      // this potentially invalidates our eventTable iterator so now we need to go and do it all again
    }

    // Notify the environment
    m_env->DestroyingLayout(this);
  }

  void Layout::zinternalPinSet(Axis axis, float mypt, const Layout *target, float targetpt, float offset /*= 0.f*/) {
    if (target && target->m_env != m_env) {
      FRAMES_LAYOUT_CHECK(false, "Attempted to constrain a frame to a frame from another environment");
      return;
    }

    if (this == target) {
      FRAMES_LAYOUT_CHECK(false, "Attempted to constrain a frame to itself");
      return;
    }

    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }

    AxisData &ax = m_axes[axis];

    AxisData::Connector &axa = ax.connections[0];
    if (axa.point_mine == mypt) {
      // We don't care if we haven't changed
      if (axa.target == target && (!target || axa.point_target == targetpt) && axa.offset == offset) {
        return;
      }

      // If we've been used, then we need to invalidate
      if (!detail::IsUndefined(axa.cached)) {
        Invalidate(axis);
      }

      if (axa.target != target) {
        if (axa.target) {
          axa.target->m_axes[axis].children.erase(this);
        }
        if (target) {
          target->m_axes[axis].children.insert(this);
        }
      }

      axa.target = target;
      axa.point_target = targetpt;
      axa.offset = offset;
      
      return;
    }

    AxisData::Connector &axb = ax.connections[1];
    if (axb.point_mine == mypt) {
      // We don't care if we haven't changed
      if (axb.target == target && (!target || axb.point_target == targetpt) && axb.offset == offset) {
        return;
      }

      // If we've been used, then we need to invalidate
      if (!detail::IsUndefined(axb.cached)) {
        Invalidate(axis);
      }

      if (axb.target != target) {
        if (axb.target) {
          axb.target->m_axes[axis].children.erase(this);
        }
        if (target) {
          target->m_axes[axis].children.insert(this);
        }
      }

      axb.target = target;
      axb.point_target = targetpt;
      axb.offset = offset;
      
      return;
    }

    bool axau = detail::IsUndefined(axa.point_mine);
    bool axbu = detail::IsUndefined(axb.point_mine);

    if (!detail::IsUndefined(ax.size_set) && (!axau || !axbu)) {
      FRAMES_LAYOUT_CHECK(target, "Frame overconstrained - attempted to assign a second point to a frame axis that already contained a size and one point");
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
    replace->target = target;
    replace->point_target = targetpt;
    replace->offset = offset;

    if (target)
    {
      target->m_axes[axis].children.insert(this);
    }

    return;
  }

  void Layout::zinternalPinClear(Axis axis, float mypt) {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }

    AxisData &ax = m_axes[axis];

    AxisData::Connector &axa = ax.connections[0];
    if (axa.point_mine == mypt) {
      if (!detail::IsUndefined(axa.cached)) {
        Invalidate(axis);
      }

      if (axa.target) {
        axa.target->m_axes[axis].children.erase(this);
      }

      axa.target = 0;
      axa.point_mine = detail::Undefined;
      axa.point_target = detail::Undefined;
      axa.offset = detail::Undefined;

      return;
    }

    AxisData::Connector &axb = ax.connections[1];
    if (axb.point_mine == mypt) {
      if (!detail::IsUndefined(axb.cached)) {
        Invalidate(axis);
      }

      if (axb.target) {
        axb.target->m_axes[axis].children.erase(this);
      }

      axb.target = 0;
      axb.point_mine = detail::Undefined;
      axb.point_target = detail::Undefined;
      axb.offset = detail::Undefined;

      return;
    }

    // If we didn't actually clear anything, no sweat, no need to invalidate
  }

  void Layout::zinternalPinClear(Anchor anchor) {
    if (anchor < 0 || anchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }

    if (!detail::IsNil(detail::AnchorLookup[anchor].x)) {
      zinternalPinClear(X, detail::AnchorLookup[anchor].x);
    }

    if (!detail::IsNil(detail::AnchorLookup[anchor].y)) {
      zinternalPinClear(Y, detail::AnchorLookup[anchor].y);
    }
  }

  void Layout::zinternalPinClear(float x, float y) {
    if (!detail::IsNil(x)) {
      zinternalPinClear(X, x);
    }

    if (!detail::IsNil(y)) {
      zinternalPinClear(Y, y);
    }
  }

  void Layout::zinternalPinClear(const Vector &position) {
    if (!detail::IsNil(position.x)) {
      zinternalPinClear(X, position.x);
    }

    if (!detail::IsNil(position.y)) {
      zinternalPinClear(Y, position.y);
    }
  }

  void Layout::zinternalPinClearAll(Axis axis) {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }
    
    AxisData &ax = m_axes[axis];

    AxisData::Connector &axa = ax.connections[0];
    if (!detail::IsUndefined(axa.point_mine)) {
      zinternalPinClear(axis, axa.point_mine);
    }

    AxisData::Connector &axb = ax.connections[1];
    if (!detail::IsUndefined(axb.point_mine)) {
      zinternalPinClear(axis, axb.point_mine);
    }

    zinternalSizeClear(axis);
  }

  // PinSet anchor adapters
  // All the anchor versions just transform themselves into no-anchor versions first
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, Anchor theiranchor) {
    if (myanchor < 0 || myanchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }

    if (theiranchor < 0 || theiranchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }

    zinternalPinSet(detail::AnchorLookup[myanchor].x, detail::AnchorLookup[myanchor].y, target, detail::AnchorLookup[theiranchor].x, detail::AnchorLookup[theiranchor].y);
  }
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, Anchor theiranchor, float xofs, float yofs) {
    if (myanchor < 0 || myanchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }

    if (theiranchor < 0 || theiranchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }
    
    zinternalPinSet(detail::AnchorLookup[myanchor].x, detail::AnchorLookup[myanchor].y, target, detail::AnchorLookup[theiranchor].x, detail::AnchorLookup[theiranchor].y, xofs, yofs);
  }
  
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, float theirx, float theiry) {
    if (myanchor < 0 || myanchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }

    zinternalPinSet(detail::AnchorLookup[myanchor].x, detail::AnchorLookup[myanchor].y, target, theirx, theiry);
  }
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, float theirx, float theiry, float xofs, float yofs) {
    if (myanchor < 0 || myanchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }
    
    zinternalPinSet(detail::AnchorLookup[myanchor].x, detail::AnchorLookup[myanchor].y, target, theirx, theiry, xofs, yofs);
  }

  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, Anchor theiranchor) {
    if (theiranchor < 0 || theiranchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }

    zinternalPinSet(myx, myy, target, detail::AnchorLookup[theiranchor].x, detail::AnchorLookup[theiranchor].y);
  }
  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, Anchor theiranchor, float xofs, float yofs) {
    if (theiranchor < 0 || theiranchor >= ANCHOR_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Anchor is invalid");
      return;
    }

    zinternalPinSet(myx, myy, target, detail::AnchorLookup[theiranchor].x, detail::AnchorLookup[theiranchor].y, xofs, yofs);
  }

  // PinSet vector adapters
  // All the vector adapters just unroll, possibly calling an anchor adapter after unrolling
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, Anchor theiranchor, const Vector &ofs) { zinternalPinSet(myanchor, target, theiranchor, ofs.x, ofs.y); }
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, const Vector &their) { zinternalPinSet(myanchor, target, their.x, their.y); }
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, const Vector &their, const Vector &ofs) { zinternalPinSet(myanchor, target, their.x, their.y, ofs.x, ofs.y); }
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, const Vector &their, float xofs, float yofs) { zinternalPinSet(myanchor, target, their.x, their.y, xofs, yofs); }
  void Layout::zinternalPinSet(Anchor myanchor, const Layout *target, float theirx, float theiry, const Vector &ofs) { zinternalPinSet(myanchor, target, theirx, theiry, ofs.x, ofs.y); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, Anchor theiranchor) { zinternalPinSet(my.x, my.y, target, theiranchor); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, Anchor theiranchor, const Vector &ofs) { zinternalPinSet(my.x, my.y, target, theiranchor, ofs.x, ofs.y); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, Anchor theiranchor, float xofs, float yofs) { zinternalPinSet(my.x, my.y, target, theiranchor, xofs, yofs); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, const Vector &their) { zinternalPinSet(my.x, my.y, target, their.x, their.y); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, const Vector &their, const Vector &ofs) { zinternalPinSet(my.x, my.y, target, their.x, their.y, ofs.x, ofs.y); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, const Vector &their, float xofs, float yofs) { zinternalPinSet(my.x, my.y, target, their.x, their.y, xofs, yofs); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, float theirx, float theiry) { zinternalPinSet(my.x, my.y, target, theirx, theiry); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, float theirx, float theiry, const Vector &ofs) { zinternalPinSet(my.x, my.y, target, theirx, theiry, ofs.x, ofs.y); }
  void Layout::zinternalPinSet(const Vector &my, const Layout *target, float theirx, float theiry, float xofs, float yofs) { zinternalPinSet(my.x, my.y, target, theirx, theiry, xofs, yofs); }
  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, Anchor theiranchor, const Vector &ofs) { zinternalPinSet(myx, myy, target, theiranchor, ofs.x, ofs.y); }
  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, const Vector &their) { zinternalPinSet(myx, myy, target, their.x, their.y); }
  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, const Vector &their, const Vector &ofs) { zinternalPinSet(myx, myy, target, their.x, their.y, ofs.x, ofs.y); }
  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, const Vector &their, float xofs, float yofs) { zinternalPinSet(myx, myy, target, their.x, their.y, xofs, yofs); }
  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, float theirx, float theiry, const Vector &ofs) { zinternalPinSet(myx, myy, target, theirx, theiry, ofs.x, ofs.y); }

  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, float theirx, float theiry) {
    if (detail::IsNil(myx) && detail::IsNil(myy)) {
      FRAMES_LAYOUT_CHECK(false, "PinSet not provided with any pin axes");
      return;
    }

    if (!target) {
      FRAMES_LAYOUT_CHECK(false, "PinSet requires offsets when pinning to origin");
      return;
    }

    if (detail::IsNil(myx) != detail::IsNil(theirx)) {
      FRAMES_LAYOUT_CHECK(false, "PinSet provided with only one pin position for X axis");
      return;
    }

    if (detail::IsNil(myy) != detail::IsNil(theiry)) {
      FRAMES_LAYOUT_CHECK(false, "PinSet provided with only one pin position for Y axis");
      return;
    }

    if (!detail::IsNil(myx)) {
      zinternalPinSet(X, myx, target, theirx);
    }

    if (!detail::IsNil(myy)) {
      zinternalPinSet(Y, myy, target, theiry);
    }
  }
  void Layout::zinternalPinSet(float myx, float myy, const Layout *target, float theirx, float theiry, float xofs, float yofs) {
    if (detail::IsNil(myx) && detail::IsNil(myy)) {
      FRAMES_LAYOUT_CHECK(false, "PinSet not provided with any pin axes");
      return;
    }

    if (target) {
      if (detail::IsNil(myx) != detail::IsNil(theirx)) {
        FRAMES_LAYOUT_CHECK(false, "PinSet provided with only one pin position for X axis");
        return;
      }

      if (detail::IsNil(myy) != detail::IsNil(theiry)) {
        FRAMES_LAYOUT_CHECK(false, "PinSet provided with only one pin position for Y axis");
        return;
      }
    } else {
      if (!detail::IsNil(theirx) != !detail::IsNil(theiry)) {
        FRAMES_LAYOUT_CHECK(false, "PinSet must have nil target anchor points when pinning to origin");
        return;
      }
    }

    if (detail::IsNil(myx) && !detail::IsNil(xofs) && xofs != 0) {
      FRAMES_LAYOUT_CHECK(false, "PinSet given non-zero X offset with nil X anchor; must have zero or nil X offset when X anchor isn't being set");
      return;
    }

    if (detail::IsNil(myy) && !detail::IsNil(yofs) && yofs != 0) {
      FRAMES_LAYOUT_CHECK(false, "PinSet given non-zero Y offset with nil Y anchor; must have zero or nil Y offset when Y anchor isn't being set");
      return;
    }

    if (!detail::IsNil(myx) && detail::IsNil(xofs)) {
      FRAMES_LAYOUT_CHECK(false, "PinSet given nil X offset with non-nil X anchor; must have non-nil offset when X anchor is being set");
      return;
    }

    if (!detail::IsNil(myy) && detail::IsNil(yofs)) {
      FRAMES_LAYOUT_CHECK(false, "PinSet given nil Y offset with non-nil Y anchor; must have non-nil offset when Y anchor is being set");
      return;
    }

    if (!detail::IsNil(myx)) {
      zinternalPinSet(X, myx, target, theirx, xofs);
    }

    if (!detail::IsNil(myy)) {
      zinternalPinSet(Y, myy, target, theiry, yofs);
    }
  }

  void Layout::zinternalSizeSet(Axis axis, float size) {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }

    AxisData &ax = m_axes[axis];

    if (!detail::IsUndefined(ax.connections[0].point_mine) && !detail::IsUndefined(ax.connections[1].point_mine)) {
      FRAMES_LAYOUT_CHECK(false, "Frame overconstrained - attempted to assign a size to a frame axis that already contained two points");
      return;
    }

    // We don't care if we haven't changed
    if (ax.size_set != size) {
      if (!detail::IsUndefined(ax.size_cached)) {
        Invalidate(axis);
      }

      ax.size_set = size;
    }
  }

  void Layout::zinternalSizeClear(Axis axis) {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }

    AxisData &ax = m_axes[axis];

    // We don't care if we haven't changed
    if (!detail::IsUndefined(ax.size_set)) {
      if (!detail::IsUndefined(ax.size_cached)) {
        Invalidate(axis);
      }

      ax.size_set = detail::Undefined;
    }
  }

  void Layout::zinternalConstraintClearAll() {
    zinternalSizeClear(X);
    zinternalSizeClear(Y);

    zinternalPinClearAll(X);
    zinternalPinClearAll(Y);
  }

  void Layout::SizeDefaultSet(Axis axis, float size) {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }

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

  void Layout::zinternalParentSet(Layout *layout) {
    if (m_parent == layout) {
      return;
    }

    Frame *frame = Cast<Frame>(this);
    if (!frame) {
      FRAMES_LAYOUT_CHECK(false, ":ParentSet() attempted on a non-frame");
      return;
    }

    if (!layout) {
      FRAMES_LAYOUT_CHECK(false, ":ParentSet() attempted with null parent");
      return;
    }

    if (layout && m_env != layout->EnvironmentGet()) {
      FRAMES_LAYOUT_CHECK(false, ":ParentSet() attempted across environment boundaries");
      return;
    }

    // First, remove ourselves from our old parent
    if (m_parent) {
      m_parent->ChildRemove(frame);
    }

    m_parent = layout;

    m_parent->ChildAdd(frame);
  }

  void Layout::zinternalLayerSet(float layer) {
    if (m_layer == layer) {
      return;
    }

    Frame *frame = Cast<Frame>(this);
    if (!frame) {
      FRAMES_LAYOUT_CHECK(false, ":LayerSet() attempted on a non-frame");
      return;
    }

    // We take ourselves out of our parents' list so we can change our layer without breaking the sort
    if (m_parent) {
      m_parent->ChildRemove(frame);
    }

    m_layer = layer;

    if (m_parent) {
      m_parent->ChildAdd(frame);
    }
  }

  void Layout::zinternalImplementationSet(bool implementation) {
    if (m_implementation == implementation) {
      return;
    }

    Frame *frame = Cast<Frame>(this);
    if (!frame) {
      FRAMES_LAYOUT_CHECK(false, ":ImplementationSet() attempted on a non-frame");
      return;
    }

    // We take ourselves out of our parents' list so we can change our strata without breaking the sort
    if (m_parent) {
      m_parent->ChildRemove(frame);
    }

    m_implementation = implementation;

    if (m_parent) {
      m_parent->ChildAdd(frame);
    }
  }

  void Layout::VisibleSet(bool visible) {
    if (m_visible == visible) {
      return;
    }

    m_visible = visible;
  }

  // The obliterate process is complicated and worthy of documenting.
  // If a layout is locked for whatever reason, the obliteration is deferred until later.
  // The Destroy event fires. This is the last point at which any functions may be called on this frame.
  // The next step is detaching - the obliterated frame, and all its children, have their event table cleared and all their constraints removed. Also, if any of those frames is the focus, the focus is cleared.
  // In theory, at this point, this section of the frame hierarchy should no longer be referred to. In practice, we want to be very careful, so we run the Extraction step.
  // Extraction tracks down all things that refer to this frame, or any of its children, spits out errors, and cleans them up appropriately.
  // Finally, the frame is entirely shut down and can be deleted.
  void Layout::zinternalObliterate() {
    // WARNING: At the moment, changes to this function must be mirrored in Environment::ObliterateUnlock. This solution sucks and should be fixed.

    if (m_env->ObliterateLocked()) {
      // The environment will do all the necessarily obliteration
      m_env->ObliterateQueue(this);
      return;
    }

    ObliterateDetach();
    ObliterateExtract();
  }

  bool Layout::EventHooked(const VerbGeneric &event) const {
    std::map<const VerbGeneric *, std::multiset<Callback, Callback::Sorter> >::const_iterator itr = m_events.find(&event);
    if (itr == m_events.end()) {
      // no handles, we're good
      return false;
    }
    
    // We now need to iterate over all events just in case they're all destroy-flagged
    const std::multiset<Callback, Callback::Sorter> &eventSet = itr->second;
    for (std::multiset<Callback, Callback::Sorter>::const_iterator itr = eventSet.begin(); itr != eventSet.end(); ++itr) {
      if (!itr->DestroyFlagGet()) {
        return true;
      }
    }
    
    // they are! sigh
    return false;
  }

  void Layout::InputModeSet(InputMode imode) {
    if (imode < 0 || imode >= IM_COUNT) {
      FRAMES_LAYOUT_CHECK(false, "Input mode is invalid");
      return;
    }

    m_inputMode = imode;
  }
  
  void Layout::ChildAdd(Frame *child) {
    m_children.insert(child);
    (child->zinternalImplementationGet() ? m_children_implementation : m_children_nonimplementation).insert(child);
  }

  void Layout::ChildRemove(Frame *child) {
    m_children.erase(child);
    (child->zinternalImplementationGet() ? m_children_implementation : m_children_nonimplementation).erase(child);
  }

  void Layout::Render(detail::Renderer *renderer) const {
    if (!renderer) {
      FRAMES_LAYOUT_CHECK(false, "Renderer is null");
      return;
    }

    if (m_visible) {
      RenderElement(renderer);

      if (!m_children.empty()) {
        RenderElementPreChild(renderer);

        for (ChildrenList::const_iterator itr = m_children.begin(); itr != m_children.end(); ++itr) {
          (*itr)->Render(renderer);
        }

        RenderElementPostChild(renderer);
      }
    }
  }

  // Technically, invalidate could be a lot more specific. There are cases where invalidating an entire axis is unnecessary - A depends on one connection, B depends on another connection, and only A's connection is invalidated.
  // It's unclear if this would be worth the additional overhead and complexity.
  // Invalidating extra stuff, while slow, is at least always correct.
  void Layout::Invalidate(Axis axis) {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }

    // see if anything needs invalidating
    const AxisData &ax = m_axes[axis];

    if (!detail::IsUndefined(ax.size_cached) || !detail::IsUndefined(ax.connections[0].cached) || !detail::IsUndefined(ax.connections[1].cached)) {
      // Do these first so we don't get ourselves trapped in an infinite loop
      ax.size_cached = detail::Undefined;
      ax.connections[0].cached = detail::Undefined;
      ax.connections[1].cached = detail::Undefined;

      for (AxisData::ChildrenList::const_iterator itr = ax.children.begin(); itr != ax.children.end(); ++itr) {
        (*itr)->Invalidate(axis);
      }

      if (m_resolved) {
        m_resolved = false;
        m_env->MarkInvalidated(this);
      }
    }
  }

  void Layout::ObliterateDetach() {
    // fire off our final event
    EventTrigger(Event::Destroy);

    // clear events so they can't fire
    m_events.clear();

    // kill my layout to unpin things
    zinternalConstraintClearAll();

    if (m_env->FocusGet() == this) {
      m_env->FocusSet(0);
    }

    // OBLITERATE ALL CHILDREN.
    for (ChildrenList::const_iterator itr = m_children.begin(); itr != m_children.end(); ++itr) {
      (*itr)->ObliterateDetach();
    }
  }

  void Layout::ObliterateExtract() {
    // at this point, nobody should be referring to me, in theory
    ObliterateExtractAxis(X);
    ObliterateExtractAxis(Y);

    // but just to make sure
    if (m_env->FocusGet() == this) {
      m_env->FocusSet(0);
    }

    // OBLITERATE ALL CHILDREN.
    for (ChildrenList::const_iterator itr = m_children.begin(); itr != m_children.end(); ) {
      // little dance here 'cause our children are going to be fucking around with our structure
      ChildrenList::const_iterator kill = itr;
      ++itr;
      (*kill)->ObliterateExtract();
    }

    // Detach ourselves from our parent
    if (m_parent) {
      Frame *frame = Cast<Frame>(this);
      if (!frame) {
        FRAMES_LAYOUT_CHECK(false, ":Obliterate() called on a non-frame that somehow has a parent");
        return;
      }

      m_parent->ChildRemove(frame);
    }
    m_parent = 0;

    // And now we're safe to delete ourselves
    m_env->ObliterateDequeue(this);
    delete this;
  }

  void Layout::ObliterateExtractAxis(Axis axis) {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }

    const AxisData &ax = m_axes[axis];
    while (!ax.children.empty()) {
      Layout *layout = *ax.children.begin();
      layout->ObliterateExtractFrom(axis, this);
    }
  }

  void Layout::ObliterateExtractFrom(Axis axis, const Layout *layout) {
    if (!(axis == X || axis == Y)) {
      FRAMES_LAYOUT_CHECK(false, "Axis is invalid");
      return;
    }

    if (!layout || layout->m_env != m_env) {
      FRAMES_LAYOUT_CHECK(false, "Obliterated layout is missing or invalid");
      return;
    }

    const AxisData &ax = m_axes[axis];

    if (ax.connections[0].target == layout) {
      FRAMES_LAYOUT_ASSERT(false, "Obliterated frame %s is still referenced by active frame %s on axis %c/%f, clearing pin", layout->DebugNameGet().c_str(), DebugNameGet().c_str(), axis ? 'Y' : 'X', ax.connections[0].point_mine);
      zinternalPinClear(axis, ax.connections[0].point_mine);
    }

    if (ax.connections[1].target == layout) {
      FRAMES_LAYOUT_ASSERT(false, "Obliterated frame %s is still referenced by active frame %s on axis %c/%f, clearing pin", layout->DebugNameGet().c_str(), DebugNameGet().c_str(), axis ? 'Y' : 'X', ax.connections[1].point_mine);
      zinternalPinClear(axis, ax.connections[1].point_mine);
    }
  }

  void Layout::Resolve() {
    float nx = LeftGet();
    RightGet();
    float ny = TopGet();
    BottomGet();
    
    float nw = WidthGet();
    float nh = HeightGet();

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

  bool Layout::Callback::Sorter::operator()(const Layout::Callback &lhs, const Layout::Callback &rhs) const {
    return lhs.m_priority < rhs.m_priority;
  }
  
  void Layout::Callback::Teardown(Environment *env) const {
    
  }

  //Layout::CallbackIterator::CallbackIterator() : m_state(STATE_COMPLETE), m_diveIndex(0), m_target(0), m_event(0) { };

  Layout::CallbackIterator::CallbackIterator(Layout *target, const VerbGeneric *event) : m_state(STATE_DIVE), m_diveIndex(0), m_target(target), m_event(event) { // set to STATE_DIVE so that NextIndex() does the right thing
    if (event->DiveGet()) {
      // Dive event! Accumulate everything we need
      Layout *ctar = target;
      while (ctar) {
        m_dives.push_back(ctar);
        ctar = ctar->ParentGet();
      }
      m_diveIndex = (int)m_dives.size();
      
      IndexNext();
    } else {
      IndexNext();
    }
    
    if (m_state != STATE_COMPLETE) {
      m_current->LockFlagIncrement();
    }
  };
  
  Layout::CallbackIterator::~CallbackIterator() {
    if (m_state != STATE_COMPLETE) {
      IteratorUnlock(m_current);
    }
  }
  
  const Layout::Callback &Layout::CallbackIterator::Get() const {
    return *m_current;
  }
  
  bool Layout::CallbackIterator::Complete() const {
    return m_state == STATE_COMPLETE;
  }
  
  void Layout::CallbackIterator::Next() {
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
  
  void Layout::CallbackIterator::IteratorUnlock(EventMultiset::iterator itr) {
    itr->LockFlagDecrement();
    if (!itr->LockFlagGet() && itr->DestroyFlagGet()) {
      // TIME TO DESTROY
      Layout *layout = m_target;
      if (m_state == STATE_DIVE || m_state == STATE_BUBBLE) {
        layout = m_dives[m_diveIndex];
      }
      
      const VerbGeneric *event = m_event;
      if (m_state == STATE_DIVE) {
        event = event->DiveGet();
      } else if (m_state == STATE_BUBBLE) {
        event = event->BubbleGet();
      }
      
      layout->EventDestroy(layout->m_events.find(event), itr);
    }
  }
  
  void Layout::CallbackIterator::IndexNext() {
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
      const VerbGeneric *event = VerbGet();
      
      // TODO: can probably be optimized by not doing a ton of lookups
      if (layout->m_events.count(event)) {
        // Sweet! We actually have events here
        m_current = layout->m_events[event].begin();
        m_last = layout->m_events[event].end();
        return;
      }
    }
  }
  
  Layout *Layout::CallbackIterator::LayoutGet() {
    if (m_state == STATE_DIVE || m_state == STATE_BUBBLE) {
      return m_dives[m_diveIndex];
    } else {
      return m_target;
    }
  }
  
  const VerbGeneric *Layout::CallbackIterator::VerbGet() {
    if (m_state == STATE_DIVE) {
      return m_event->DiveGet();
    } else if (m_state == STATE_BUBBLE) {
      return m_event->BubbleGet();
    } else {
      return m_event;
    }
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
}

