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

  typedef intptr_t EventId;

  /// Base class for all objects that have a position within the Frames system.
  /**
  This is the base class of Frame, which is the main element of Frames UIs. Layout itself is immutable - it's generally used for system objects such as Root, which represents the screen. Frame exposes many of its protected members as public.

  All Layouts are associated with exactly one Environment.
  */
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
    /// Retrieves human-readable type for this class.
    static const char *GetStaticType();
    /// Retrieves human-readable type for this object.
    /** Must be overloaded for subclasses; see Texture for an example. */
    virtual const char *GetType() const { return GetStaticType(); }

    /// Gets the location of a given position on a given axis.
    /**
    If axis is X, 0 represents the left edge of the frame, 1 represents the right edge.
    If axis is Y, 0 represents the top edge of the frame, 1 represents the bottom edge.
    Fractional positions are allowed; for example, 0.5 represents the center. Coordinates less than 0 or greater than 1 are also allowed and will return a linear extrapolation of the frame's coordinates. */
    float GetPoint(Axis axis, float pt) const;
    /// Gets the location of the left edge of the layout.
    float GetLeft() const { return GetPoint(X, 0); }
    /// Gets the location of the right edge of the layout.
    float GetRight() const { return GetPoint(X, 1); }
    /// Gets the location of the top edge of the layout.
    float GetTop() const { return GetPoint(Y, 0); }
    /// Gets the location of the bottom edge of the layout.
    float GetBottom() const { return GetPoint(Y, 1); }
    /// Gets the bounds of the layout.
    Rect GetBounds() const;

    /// Gets the size of the layout on a given axis.
    float GetSize(Axis axis) const;
    /// Gets the width of the layout.
    float GetWidth() const { return GetSize(X); }
    /// Gets the height of the layout.
    float GetHeight() const { return GetSize(Y); }

    /// Gets the parent of the layout. Will be null for the Root layout, non-null for all other layouts.
    Layout *GetParent() const { return m_parent; }

    /// Gets the input-accepting layout under the given coordinates.
    Layout *GetLayoutUnder(float x, float y);

    // RetrieveHeight/RetrieveWidth/RetrievePoint/etc?

    /// Retrieves the name of this layout.
    const std::string &GetName() const { return m_name; }

    /// Type used to store Children. Conforms to the interface of std::set.
    typedef std::set<Layout *, FrameOrderSorter> ChildrenList;
    /// Retrieves a list of this layout's children.
    const ChildrenList &GetChildren() { return m_children; }
    
    /// Gets this layout's environment.
    Environment *GetEnvironment() const { return m_env; }

    // --------------- Events

    /// Attaches an event handler to a verb.
    template <typename Parameters> void EventAttach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority = 0.0);
    /// Detaches an event handler from a verb.
    template <typename Parameters> void EventDetach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority = detail::Undefined);
    
    /// Triggers the event handlers attached to a verb.
    inline void EventTrigger(const Verb<void ()> &event);
    /// Triggers the event handlers attached to a verb.
    template <typename P1> void EventTrigger(const Verb<void (P1)> &event, typename detail::MakeConstRef<P1>::T p1);

    // --------------- Lua
    
    /// Pushes this layout's Lua handle onto a Lua stack.
    void luaF_push(lua_State *L) const;

    // --------------- Debug

    /// Dumps the detailed layout information to the debug log.
    void DebugDumpLayout() const;

    /// Returns an extremely detailed name for this layout, including the names of its parents.
    std::string DebugGetName() const;    // THIS MIGHT BE VERY, VERY SLOW

  protected:
    /// Constructor for Layout.
    Layout(const std::string &name, Layout *parent, Environment *env = 0);
    virtual ~Layout();

    /// Replace or create an anchor.
    void SetPoint(Axis axis, float mypt, const Layout *link, float theirpt, float offset = 0.f);
    /// Clear an existing anchor.
    void ClearPoint(Axis axis, float mypt);
    /// Clear an existing anchor.
    void ClearPoint(Anchor anchor);
    /// Clear all anchors on an axis.
    void ClearAllPoints(Axis axis);

    // SetPoint variants
    // Two-anchor version
    /// Replace or create an anchor.
    void SetPoint(Anchor myanchor, const Layout *link, Anchor theiranchor);
    /// Replace or create an anchor.
    void SetPoint(Anchor myanchor, const Layout *link, Anchor theiranchor, float xofs, float yofs);
    // First-anchor version
    /// Replace or create an anchor.
    void SetPoint(Anchor myanchor, const Layout *link, float theirx, float theiry);
    /// Replace or create an anchor.
    void SetPoint(Anchor myanchor, const Layout *link, float theirx, float theiry, float xofs, float yofs);
    // Second-anchor version
    /// Replace or create an anchor.
    void SetPoint(float myx, float myy, const Layout *link, Anchor theiranchor);
    /// Replace or create an anchor.
    void SetPoint(float myx, float myy, const Layout *link, Anchor theiranchor, float xofs, float yofs);
    // No-anchor version
    /// Replace or create an anchor.
    void SetPoint(float myx, float myy, const Layout *link, float theirx, float theiry);
    /// Replace or create an anchor.
    void SetPoint(float myx, float myy, const Layout *link, float theirx, float theiry, float xofs, float yofs);

    /// Set the size along an axis.
    void SetSize(Axis axis, float size);
    /// Set the width.
    void SetWidth(float size) { return SetSize(X, size); }
    /// Set the height.
    void SetHeight(float size) { return SetSize(Y, size); }
    /// Clear a previously set size along an axis.
    void ClearSize(Axis axis);

    /// Clear all anchors and sizes.
    void ClearConstraints();

    /// Set the default size along an axis.
    void SetSizeDefault(Axis axis, float size);
    /// Set the default width.
    void SetWidthDefault(float size) { return SetSizeDefault(X, size); }
    /// Set the default height.
    void SetHeightDefault(float size) { return SetSizeDefault(Y, size); }

    /// Set this frame's parent.
    /** The new parent must have the same environment as this layout. */
    void SetParent(Layout *layout);

    /// Sets the name of this layout.
    void SetName(const std::string &name) { m_name = name; }

    void SetLayer(float layer);
    float GetLayer() const { return m_layer; }

    void SetStrata(float strata);
    float GetStrata() const { return m_strata; }

    void SetVisible(bool visible);
    bool GetVisible() const { return m_visible; }

    void SetAlpha(float alpha) { m_alpha = alpha; }
    float GetAlpha() const { return m_alpha; }

    // This is for further-down classes, not so useful for users
    void SetFullMouseMasking(bool mask) { m_fullMouseMasking = mask; }
    bool GetFullMouseMasking() { return m_fullMouseMasking; }
    virtual bool TestMouseMasking(float x, float y) { return true; }

    void Obliterate(); // prep for destruction along with all children

    virtual void RenderElement(detail::Renderer *renderer) const { };
    virtual void RenderElementPost(detail::Renderer *renderer) const { };

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
    void Render(detail::Renderer *renderer) const;

    // Layout engine
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
    float m_strata;
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
