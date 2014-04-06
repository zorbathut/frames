// The Layout class

#ifndef FRAMES_LAYOUT
#define FRAMES_LAYOUT

#include "frames/delegate.h"
#include "frames/detail.h"
#include "frames/event.h"
#include "frames/lua.h"
#include "frames/input.h"
#include "frames/noncopyable.h"
#include "frames/point.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <set>
#include <map>
#include <string>

struct lua_State;

namespace Frames {
  class Environment;
  struct Rect;
  class Layout;

  namespace detail {
    class Renderer;
  }

  /// Base class containing location and layout information for all Frames elements.
  /**
  A Layout includes all information needed to calculate and update the position and ordering of an element in an Environment. The vast majority of all Layouts are actually \ref Frame "Frames"; the only exception is the Root layout.
  
  Layout includes all update-related functions as protected members, most of which are exposed by Frame.*/
  class Layout : detail::Noncopyable {
  public:
    FRAMES_VERB_DECLARE_BEGIN
      /// Signals when a frame's edges move.
      /** WARNING: Unlike most events, Move may not signal immediately after a state change. Move is guaranteed to fire before Environment::Prepare() returns. */
      FRAMES_VERB_DECLARE(Move, ());

      /// Signals when a frame's size changes.
      /** Since size cannot change without the frame's edges moving, a Size signal is always associated with a Move signal.
      
      WARNING: Unlike most events, Size may not signal immediately after a state change. Size is guaranteed to fire before Environment::Prepare() returns. */
      FRAMES_VERB_DECLARE(Size, ());

      /// Signals when the mouse enters a frame.
      /** If a mouse moves from one frame to another, MouseOver is guaranteed to fire after MouseOut. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseOver, ());

      /// Signals when the mouse moves while inside a frame.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMove, (const Point &pt));

      /// Signals when the mouse moves while outside a frame, after pressing a button inside that frame and keeping it held.
      /** This functionality is intended for drag-and-drop applications. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseMoveoutside, (const Point &pt));

      /// Signals when the mouse leaves a frame.
      /** If a mouse moves from one frame to another, MouseOver is guaranteed to fire after MouseOut. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseOut, ());

      /// Signals immediately after MouseButtonUp(0). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseLeftUp, ());
      /// Signals immediately after MouseButtonUpoutside(0). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseLeftUpoutside, ());
      /// Signals immediately after MouseButtonDown(0). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseLeftDown, ());
      /// Signals immediately after MouseButtonClick(0). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseLeftClick, ());

      /// Signals immediately after MouseButtonUp(1). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMiddleUp, ());
      /// Signals immediately after MouseButtonUpoutside(1). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMiddleUpoutside, ());
      /// Signals immediately after MouseButtonDown(1). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMiddleDown, ());
      /// Signals immediately after MouseButtonClick(1). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMiddleClick, ());

      /// Signals immediately after MouseButtonUp(2). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseRightUp, ());
      /// Signals immediately after MouseButtonUpoutside(2). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseRightUpoutside, ());
      /// Signals immediately after MouseButtonDown(2). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseRightDown, ());
      /// Signals immediately after MouseButtonClick(2). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseRightClick, ());

      /// Signals that a mouse button has been released while inside a frame.
      FRAMES_VERB_DECLARE_BUBBLE(MouseButtonUp, (int button));
      /// Signals that a mouse button has been released while outside a frame, after pressing that button inside that frame and then moving the mouse out of the frame.
      /** This functionality is intended for drag-and-drop applications. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseButtonUpoutside, (int button));
      /// Signals that a mouse button has been pressed while inside a frame.
      FRAMES_VERB_DECLARE_BUBBLE(MouseButtonDown, (int button));
      /// Signals that a mouse button has been clicked while inside a frame.
      /** This is currently defined as "press, then release, while inside the same frame", and is provided as a commonly-needed convenience verb. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseButtonClick, (int button));

      /// Signals that the mouse wheel has been moved while inside a frame.
      FRAMES_VERB_DECLARE_BUBBLE(MouseWheel, (int delta));

      /// Signals that a key has been pressed while a frame has the key focus.
      /** This verb is recommended for non-text applications, as it reports the key ID and not the unicode text that was entered. */
      FRAMES_VERB_DECLARE_BUBBLE(KeyDown, (const Key &kev));
      /// Signals that text has been typed while a frame has the key focus.
      /** This verb is recommended for text applications, as it provides a Unicode string potentially containing multiple entered characters.*/
      FRAMES_VERB_DECLARE_BUBBLE(KeyType, (const std::string &text));
      /// Signals that a keypress has repeated while a frame has the key focus.
      /** This verb is recommended for non-text applications, as it reports the key ID and not the unicode text that was entered. */
      FRAMES_VERB_DECLARE_BUBBLE(KeyRepeat, (const Key &kev));
      /// Signals that a key has been released while a frame has the key focus.
      /** This verb is recommended for non-text applications, as it reports the key ID and not the unicode text that was entered. */
      FRAMES_VERB_DECLARE_BUBBLE(KeyUp, (const Key &kev));
    FRAMES_VERB_DECLARE_END

  private:
    friend class Environment;
    friend class Mask; // solely for MouseMasking
    
    struct FrameOrderSorter { bool operator()(const Layout *lhs, const Layout *rhs) const; };
    
    // Event system
    
    // Must function properly when copied by value!
    struct FECallback {
    public:
      // NOTE: this works because delegate is POD
      FECallback() : m_type(TYPE_INVALID), m_priority(0), m_destroy(false), m_lock(0) { }
      ~FECallback() { }
      
      template<typename T> static FECallback CreateNative(Delegate<T> din, float priority) {
        BOOST_STATIC_ASSERT(sizeof(Delegate<T>) == sizeof(Delegate<void ()>));
        
        FECallback rv;
        rv.m_priority = priority;
        
        rv.m_type = TYPE_NATIVE;
        memcpy(rv.c.nativeDelegate, &din, sizeof(din)); // TODO: this is probably slower than necessary, but type aliasing makes it a nightmare otherwise
        
        return rv;
      }
      
      static FECallback CreateLua(lua_State *L, int handle, float priority) {
        FECallback rv;
        rv.m_priority = priority;
        
        rv.m_type = TYPE_LUA;
        rv.c.lua.L = L;
        rv.c.lua.handle = handle;
        
        return rv;
      }

      struct Sorter {
        bool operator()(const FECallback &lhs, const FECallback &rhs) const;
      };
      
      void Call(Handle *eh) const {
        if (m_type == TYPE_NATIVE) {
          // TODO: This is *definitely* slower than necessary. Fix this!
          Delegate<void (Handle *)> dg;
          memcpy(&dg, c.nativeDelegate, sizeof(dg));
          dg(eh);
        } else if (m_type == TYPE_LUA) {
          int stackfront = luaF_prepare(eh);
          luaF_call(stackfront);
        }
      }
      
      template <typename P1> void Call(Handle *eh, P1 p1) const {
        if (m_type == TYPE_NATIVE) {
          // TODO: This is *definitely* slower than necessary. Fix this!
          Delegate<void (Handle *, P1)> dg;
          memcpy(&dg, c.nativeDelegate, sizeof(dg));
          dg(eh, p1);
        } else if (m_type == TYPE_LUA) {
          int stackfront = luaF_prepare(eh);
          ::Frames::luaF_push(c.lua.L, p1);
          luaF_call(stackfront);
        }
      }
      
      bool NativeIs() const { return m_type == TYPE_NATIVE; }
      template<typename T> bool NativeCallbackEqual(Delegate<T> din) const {
        return NativeIs() && memcmp(&din, c.nativeDelegate, sizeof(din)) == 0;
      }
      
      bool LuaIs() const { return m_type == TYPE_LUA; }
      bool LuaEnvironmentEqual(lua_State *L) const {
        return LuaIs() && c.lua.L == L;
      }
      bool LuaHandleEqual(lua_State *L, int handle) const {
        return LuaIs() && c.lua.L == L && c.lua.handle == handle;
      }
      
      float PriorityGet() const { return m_priority; }
      
      bool DestroyFlagGet() const { return m_destroy; }
      void DestroyFlagSet() const { m_destroy = true; }
      
      bool LockFlagGet() const { return m_lock != 0; }
      void LockFlagIncrement() const { ++m_lock; }  // must store result and pass it to LockFlagDecrement
      void LockFlagDecrement() const { --m_lock; }
      
      void Teardown(Environment *env) const;  // cleans up the underlying resources, if any. Not the same as a destructor! This isn't RAII for efficiency reasons. Environment provided for debug hooks.
      
    private:
      enum { TYPE_ERASED, TYPE_INVALID, TYPE_NATIVE, TYPE_LUA } m_type;
      
      // the delegate itself - this is not the right type, it is casted appropriately by Call
      union {
        char nativeDelegate[sizeof(Delegate<void ()>)];
        struct {
          lua_State *L;
          int handle;
        } lua;
      } c;

      float m_priority;
      
      // "mutable" because they don't contribute to the sorting, so we need them to be modifiable even when used as multiset key
      mutable bool m_destroy;
      mutable int m_lock;
      
      // Lua infrastructure
      int luaF_prepare(Handle *eh) const;  // prepares the function pointer and early parameters
      void luaF_call(int stackfront) const;
    };
    
    typedef std::multiset<FECallback, FECallback::Sorter> EventMultiset;
    typedef std::map<const detail::VerbBase *, std::multiset<FECallback, FECallback::Sorter> > EventLookup;
    
    class FEIterator {
    public:
      FEIterator();
      FEIterator(Layout *target, const detail::VerbBase *event);
      FEIterator(const FEIterator &itr);
      void operator=(const FEIterator &itr);
      ~FEIterator();
      
      const FECallback &Get() const;
      bool Complete() const;
      void Next();
      
    private:
      void IteratorUnlock(EventMultiset::iterator itr); // iterator may be invalidated by this function
      void IndexNext();
      
      Layout *LayoutGet();
      const detail::VerbBase *EventGet();
      
      enum State { STATE_DIVE, STATE_MAIN, STATE_BUBBLE, STATE_COMPLETE };
      State m_state;
      
      std::vector<Layout *> m_dives;
      int m_diveIndex;
      
      Layout *m_target;
      const detail::VerbBase *m_event;
      
      EventMultiset::iterator m_current;
      EventMultiset::iterator m_last;
    };
    
  public:
    // --------- Typing and identification

    /// Returns a human-readable type string for this class.
    static const char *GetStaticType();
    /// Returns a human-readable type string for this instance.
    /** Must be overloaded in all subclasses (see Texture for an example). */
    virtual const char *GetType() const { return GetStaticType(); }

    /// Returns the name of this layout.
    const std::string &GetName() const { return m_name; }

    // --------- Layout accessors

    /// Returns the position of an anchor along a given axis.
    /** See \ref layoutbasics for details. */
    float GetPoint(Axis axis, float pt) const;
    /// Returns the left edge's position.
    float GetLeft() const { return GetPoint(X, 0); }
    /// Returns the right edge's position.
    float GetRight() const { return GetPoint(X, 1); }
    /// Returns the top edge's position.
    float GetTop() const { return GetPoint(Y, 0); }
    /// Returns the bottom edge's position.
    float GetBottom() const { return GetPoint(Y, 1); }
    /// Returns the bounds of the layout.
    Rect GetBounds() const;

    /// Returns the size along a given axis.
    float GetSize(Axis axis) const;
    /// Returns the width.
    float GetWidth() const { return GetSize(X); }
    /// Returns the height.
    float GetHeight() const { return GetSize(Y); }

    /// Returns the parent. Guaranteed to be non-null unless this is Root.
    Layout *GetParent() const { return m_parent; }

    /// Returns the highest input-accepting frame located at the given coordinates.
    Layout *GetLayoutUnder(float x, float y);

    // RetrieveHeight/RetrieveWidth/RetrievePoint/etc?

    /// Type used to store a list of children, sorted from bottom to top. Conforms to std::set's interface; may not be a std::set.
    typedef std::set<Layout *, FrameOrderSorter> ChildrenList;

    /// Returns the children of this frame.
    const ChildrenList &GetChildren() { return m_children; }
    
    // --------- State

    /// Sets the visibility flag.
    /** Invisible frames will not render or accept input, nor will their children render nor accept input. */
    void SetVisible(bool visible);
    /// Gets the visibility flag.
    bool GetVisible() const { return m_visible; }

    /// Sets the alpha multiplier.
    /** Changing the alpha multiplier causes this frame, and all its children, to render partially transparent. Note that this is done on a per-frame basis and will not result in layer-perfect results. */
    void SetAlpha(float alpha) { m_alpha = alpha; }
    /// Gets the alpha multiplier.
    float GetAlpha() const { return m_alpha; }

    // --------- Events

    /// Attaches a handler to a verb on this layout.
    template <typename Parameters> void EventAttach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority = 0.0);
    /// Detaches a handler from a verb on this layout.
    template <typename Parameters> void EventDetach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority = detail::Undefined);
    
    /// Triggers all attached handlers for a given verb.
    inline void EventTrigger(const Verb<void ()> &event);
    /// Triggers all attached handlers for a given verb.
    template <typename P1> void EventTrigger(const Verb<void (P1)> &event, typename detail::MakeConstRef<P1>::T p1);

    /// Returns the environment.
    Environment *GetEnvironment() const { return m_env; }

    // --------- Lua

    /// Pushes a handle to this layout onto a Lua stack.
    void luaF_push(lua_State *L) const;

    // --------- Debug

    /// Dumps comprehensive layout information to the debug log.
    void DebugDumpLayout() const;

    /// Constructs and return the full named path of this layout.
    std::string DebugGetName() const;

  protected:
    /// Constructor for layouts with a parent.
    Layout(const std::string &name, Layout *parent);
    virtual ~Layout();

    // --------- SetPoint

    /// Creates or redefines a single anchor \ref layoutbasics "link".
    /** It is generally recommended to use the other SetPoint overloads before this one. See \ref layoutbasics for more information. */
    void SetPoint(Axis axis, float mypt, const Layout *link, float theirpt, float offset = 0.f);
    /// Clears a single \ref layoutbasics "link".
    void ClearPoint(Axis axis, float mypt);
    /// Clears a single \ref layoutbasics "link".
    void ClearPoint(Anchor anchor);
    /// Clears all \ref layoutbasics "links" on an axis.
    void ClearAllPoints(Axis axis);

    // --------- SetPoint variants

    /// SetPoint variant for \ref layoutbasics "linking" two \ref Anchor "Anchors".
    void SetPoint(Anchor myanchor, const Layout *link, Anchor theiranchor);
    /// SetPoint variant for \ref layoutbasics "linking" two \ref Anchor "Anchors" with an offset.
    void SetPoint(Anchor myanchor, const Layout *link, Anchor theiranchor, float xofs, float yofs);
    /// SetPoint variant for \ref layoutbasics "linking" an Anchor to a relative point.
    void SetPoint(Anchor myanchor, const Layout *link, float theirx, float theiry);
    /// SetPoint variant for \ref layoutbasics "linking" an Anchor to a relative point with an offset.
    void SetPoint(Anchor myanchor, const Layout *link, float theirx, float theiry, float xofs, float yofs);
    /// SetPoint variant for \ref layoutbasics "linking" a relative point to an Anchor.
    void SetPoint(float myx, float myy, const Layout *link, Anchor theiranchor);
    /// SetPoint variant for \ref layoutbasics "linking" a relative point to an Anchor with an offset.
    void SetPoint(float myx, float myy, const Layout *link, Anchor theiranchor, float xofs, float yofs);
    /// SetPoint variant for \ref layoutbasics "linking" two relative points.
    void SetPoint(float myx, float myy, const Layout *link, float theirx, float theiry);
    /// SetPoint variant for \ref layoutbasics "linking" two relative points with an offset.
    void SetPoint(float myx, float myy, const Layout *link, float theirx, float theiry, float xofs, float yofs);

    /// Sets the size of an axis.
    void SetSize(Axis axis, float size);
    /// Sets the width.
    void SetWidth(float size) { return SetSize(X, size); }
    /// Sets the height.
    void SetHeight(float size) { return SetSize(Y, size); }
    /// Clears the size of an axis.
    void ClearSize(Axis axis);

    /// Clears all links and sizes.
    void ClearConstraints();

    /// Set default size of an axis.
    /** Default sizing is used for frames that semantically have a "normal" size, but may be resized arbitrary by the user. The frame will fall back to the default size if its size is left undefined otherwise (either via explicit sizing or via sufficient linking to fix the size.)
    
    This should generally not be exposed to the end-user of a class - it is intended as an internal implementation tool. */
    void SetSizeDefault(Axis axis, float size);
    /// Set default width. See SetSizeDefault for details.
    void SetWidthDefault(float size) { return SetSizeDefault(X, size); }
    /// Set default height. See SetSizeDefault for details.
    void SetHeightDefault(float size) { return SetSizeDefault(Y, size); }

    /// Sets the parent.
    /** New parent must be non-null and a member of the same environment. */
    void SetParent(Layout *layout);

    /// Sets the name.
    void SetName(const std::string &name) { m_name = name; }

    /// Sets the layer.
    /** Layers are used to determine frame render and input order. Higher-numbered layers are layered on top of lower-numbered layers. All floating-point values are acceptable besides infinities and NaN. */
    void SetLayer(float layer);
    /// Gets the layer.
    float GetLayer() const { return m_layer; }

    /// Sets the implementation flag.
    /** The implementation flag is used to create new frame types that are composited out of subframes. All frames with this flag set will be rendered before frames without the flag set. In addition, frames with this flag will not be returned from GetChildren() (TODO: nyi!). */
    void SetImplementation(bool implementation);
    /// Gets the implementation flag.
    bool GetImplementation() const { return m_implementation; }

    /// Destroys this frame and all its children.
    /** Also destroys all \ref layoutbasics "links" to and from these layouts. It is undefined behavior to refer to this frame or any of its children after this function is called. */
    void Obliterate();

    /// Called when this frame is rendered.
    /** Overload this to create your own frame types. Must call (super)::RenderElement before it does its own work. */
    virtual void RenderElement(detail::Renderer *renderer) const { };
    /// Called before this frame's children have been rendered.
    /** Overload this for pre-render setup. If this frame has no children, may not be called at all. Must call (super)::RenderElementPreChild before it does its own work. */
    virtual void RenderElementPreChild(detail::Renderer *renderer) const {};
    /// Called after this frame's children have been rendered.
    /** Overload this for post-render cleanup. If this frame has no children, may not be called at all. Must call (super)::RenderElementPreChild *after* it does its own work. */
    virtual void RenderElementPostChild(detail::Renderer *renderer) const { };

    // make sure you call these down if you override them
    bool EventHookedIs(const detail::VerbBase &event) const;
        
    virtual void EventAttached(const detail::VerbBase *id);
    virtual void EventDetached(const detail::VerbBase *id);
    
    // Lua
    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, GetStaticType()); } // see Layout::luaF_Register for what yours should look like
    void luaF_RegisterWorker(lua_State *L, const char *name) const;

    static void luaF_RegisterFunctions(lua_State *L);

    static void luaF_RegisterFunction(lua_State *L, const char *owner, const char *name, int (*func)(lua_State *));

  private:
    /// Null constructor used to create Root.
    Layout(Environment *env);

    void Render(detail::Renderer *renderer) const;

    // Provided for Mask.
    void SetFullMouseMasking(bool mask) { m_fullMouseMasking = mask; }
    bool GetFullMouseMasking() { return m_fullMouseMasking; }
    virtual bool TestMouseMasking(float x, float y) { return true; }

    // Layout engine - note that this is used heavily by Frame!
    void Invalidate(Axis axis);
    void Obliterate_Detach(); // Detach this layout from all layouts
    void Obliterate_Extract();  // Detach everything that refers to this layout
    void Obliterate_Extract_Axis(Axis axis);  // Detach everything that refers to this axis
    void Obliterate_Extract_From(Axis axis, const Layout *layout);
    void Resolve();
    struct AxisData {
      AxisData() : size_cached(detail::Undefined), size_set(detail::Undefined), size_default(40) { };

      mutable float size_cached;

      struct Connector {
        Connector() : link(0), point_mine(detail::Undefined), point_link(detail::Undefined), offset(detail::Undefined), cached(detail::Undefined) { };

        const Layout *link;
        float point_mine;
        float point_link;
        float offset;

        mutable float cached;
      };
      Connector connections[2];

      float size_set;
      float size_default;

      typedef std::multiset<Layout *> ChildrenList;
      mutable ChildrenList children;
    };
    AxisData m_axes[2];
    mutable bool m_resolved;  // whether *this* frame has its layout completely determined

    // Layout events
    mutable float m_last_width, m_last_height;
    mutable float m_last_x, m_last_y;

    // Layer/parenting engine
    float m_layer;
    bool m_implementation;
    unsigned int m_constructionOrder; // This is used to create consistent results when frames are Z-conflicting
    Layout *m_parent;
    bool m_visible;
    ChildrenList m_children;

    // Rendering effects
    float m_alpha;

    // Input
    bool m_fullMouseMasking;
    bool m_acceptInput;

    // Naming system
    std::string m_name;
    
    // Event system
    EventLookup m_events;
    void EventDestroy(EventLookup::iterator eventTable, EventMultiset::iterator toBeRemoved);
    
    void luaF_ClearEvents_Recursive(lua_State *L);

    // Obliterate buffering (sort of related to the event system) - todo make this take up less space
    int m_obliterate_lock;
    bool m_obliterate_buffered;
    void Obliterate_Lock();
    void Obliterate_Unlock(); // note that, after calling this function, the frame might eat itself

    // Global environment
    Environment *m_env;

    // Lua bindings
    static int luaF_GetLeft(lua_State *L);
    static int luaF_GetRight(lua_State *L);
    static int luaF_GetTop(lua_State *L);
    static int luaF_GetBottom(lua_State *L);
    static int luaF_GetBounds(lua_State *L);

    static int luaF_GetWidth(lua_State *L);
    static int luaF_GetHeight(lua_State *L);

    static int luaF_GetChildren(lua_State *L);

    static int luaF_GetName(lua_State *L);
    static int luaF_GetNameFull(lua_State *L);
    static int luaF_GetType(lua_State *L);
    
    static int luaF_EventAttach(lua_State *L);
    static int luaF_EventDetach(lua_State *L);
    
    static int luaF_DebugDumpLayout(lua_State *L);
  };

  // Debug code
  #ifdef _MSC_VER
    #define FRAMES_LAYOUT_ASSERT(x, errstring, ...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(detail::Format(errstring, __VA_ARGS__))))
    #define FRAMES_LAYOUT_CHECK(x, errstring, ...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(detail::Format(errstring, __VA_ARGS__))))

    #define FRAMES_ERROR(...) GetEnvironment()->LogError(detail::Format(__VA_ARGS__))
    #define FRAMES_DEBUG(...) GetEnvironment()->LogDebug(detail::Format(__VA_ARGS__))
  #else
    #define FRAMES_LAYOUT_ASSERT(x, errstring, args...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(detail::Format(errstring, ## args))))
    #define FRAMES_LAYOUT_CHECK(x, errstring, args...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(detail::Format(errstring, ## args))))

    #define FRAMES_ERROR(args...) GetEnvironment()->LogError(detail::Format(args))
    #define FRAMES_DEBUG(args...) GetEnvironment()->LogDebug(detail::Format(args))
  #endif
}

#include "frames/layout_template_inline.h"

#endif
