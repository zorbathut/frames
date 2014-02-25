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

struct lua_State;

namespace Frames {
  class Environment;
  struct Rect;
  class Renderer;
  class Layout;
  
  FRAMES_FRAMEEVENT_DECLARE(Move, ());
  FRAMES_FRAMEEVENT_DECLARE(Size, ());

  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseOver, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMove, (const Point &pt));
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMoveoutside, (const Point &pt));
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseOut, ());

  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseLeftUp, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseLeftUpoutside, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseLeftDown, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseLeftClick, ());

  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMiddleUp, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMiddleUpoutside, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMiddleDown, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMiddleClick, ());

  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseRightUp, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseRightUpoutside, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseRightDown, ());
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseRightClick, ());

  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseButtonUp, (int button));
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseButtonUpoutside, (int button));
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseButtonDown, (int button));
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseButtonClick, (int button));

  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseWheel, (int delta));

  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(KeyDown, (const KeyEvent &kev));
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(KeyType, (const std::string &text));
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(KeyRepeat, (const KeyEvent &kev));
  FRAMES_FRAMEEVENT_DECLARE_BUBBLE(KeyUp, (const KeyEvent &kev));

  typedef intptr_t EventId;

  class Layout : Noncopyable {
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
      
      void Call(EventHandle *eh) const {
        if (m_type == TYPE_NATIVE) {
          // TODO: This is *definitely* slower than necessary. Fix this!
          Delegate<void (EventHandle *)> dg;
          memcpy(&dg, c.nativeDelegate, sizeof(dg));
          dg(eh);
        } else if (m_type == TYPE_LUA) {
          int stackfront = luaF_prepare(eh);
          luaF_call(stackfront);
        }
      }
      
      template <typename P1> void Call(EventHandle *eh, P1 p1) const {
        if (m_type == TYPE_NATIVE) {
          // TODO: This is *definitely* slower than necessary. Fix this!
          Delegate<void (EventHandle *, P1)> dg;
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
      int luaF_prepare(EventHandle *eh) const;  // prepares the function pointer and early parameters
      void luaF_call(int stackfront) const;
    };
    
    typedef std::multiset<FECallback, FECallback::Sorter> EventMultiset;
    typedef std::map<const EventTypeBase *, std::multiset<FECallback, FECallback::Sorter> > EventLookup;
    
    class FEIterator {
    public:
      FEIterator();
      FEIterator(Layout *target, const EventTypeBase *event);
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
      const EventTypeBase *EventGet();
      
      enum State { STATE_DIVE, STATE_MAIN, STATE_BUBBLE, STATE_COMPLETE };
      State m_state;
      
      std::vector<Layout *> m_dives;
      int m_diveIndex;
      
      Layout *m_target;
      const EventTypeBase *m_event;
      
      EventMultiset::iterator m_current;
      EventMultiset::iterator m_last;
    };
    
  public:
    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }

    float GetPoint(Axis axis, float pt) const;
    float GetLeft() const { return GetPoint(X, 0); }
    float GetRight() const { return GetPoint(X, 1); }
    float GetTop() const { return GetPoint(Y, 0); }
    float GetBottom() const { return GetPoint(Y, 1); }
    Rect GetBounds() const;

    float GetSize(Axis axis) const;
    float GetWidth() const { return GetSize(X); }
    float GetHeight() const { return GetSize(Y); }

    Layout *GetParent() const { return m_parent; }

    Layout *GetFrameUnder(float x, float y);

    // RetrieveHeight/RetrieveWidth/RetrievePoint/etc?

    const char *GetNameStatic() const { return m_name_static; }
    void SetNameStatic(const char *name) { m_name_static = name; }  // WARNING: This does not make a copy! The const char* must have a lifetime longer than this frame.

    int GetNameId() const { return m_name_id; }
    void SetNameId(int id) { m_name_id = id; }

    const std::string &GetNameDynamic() const { return m_name_dynamic; }
    void SetNameDynamic(const std::string &name) { m_name_dynamic = name; }

    typedef std::set<Layout *, FrameOrderSorter> ChildrenList;
    const ChildrenList &GetChildren() { return m_children; }
    
    // Events
    template <typename Parameters> void EventAttach(const EventType<Parameters> &event, typename EventType<Parameters>::TypeDelegate handler, float priority = 0.0);
    template <typename Parameters> void EventDetach(const EventType<Parameters> &event, typename EventType<Parameters>::TypeDelegate handler, float priority = Utility::Undefined);
    
    inline void EventTrigger(const EventType<void ()> &event);
    template <typename P1> void EventTrigger(const EventType<void (P1)> &event, typename Utility::MakeConstRef<P1>::T p1);

    Environment *GetEnvironment() const { return m_env; }

    // Lua-specific
    void luaF_push(lua_State *L) const;

    std::string GetName() const;
    std::string GetNameFull() const;    // THIS MIGHT BE VERY, VERY SLOW

    // Debug
    void DebugDumpLayout() const;

  protected:
    Layout(Layout *parent, Environment *env = 0);
    virtual ~Layout();

    // while Layout isn't mutable, things that inherit from Layout might be
    void SetPoint(Axis axis, float mypt, const Layout *link, float theirpt, float offset = 0.f);
    void ClearPoint(Axis axis, float mypt);
    void ClearPoint(Anchor anchor);
    void ClearAllPoints(Axis axis);
    void ClearAllPoints();

    // SetPoint variants
    // Two-anchor version
    void SetPoint(Anchor myanchor, const Layout *link, Anchor theiranchor);
    void SetPoint(Anchor myanchor, const Layout *link, Anchor theiranchor, float xofs, float yofs);
    // First-anchor version
    void SetPoint(Anchor myanchor, const Layout *link, float theirx, float theiry);
    void SetPoint(Anchor myanchor, const Layout *link, float theirx, float theiry, float xofs, float yofs);
    // Second-anchor version
    void SetPoint(float myx, float myy, const Layout *link, Anchor theiranchor);
    void SetPoint(float myx, float myy, const Layout *link, Anchor theiranchor, float xofs, float yofs);
    // No-anchor version
    void SetPoint(float myx, float myy, const Layout *link, float theirx, float theiry);
    void SetPoint(float myx, float myy, const Layout *link, float theirx, float theiry, float xofs, float yofs);

    void SetSize(Axis axis, float size);
    void SetWidth(float size) { return SetSize(X, size); }
    void SetHeight(float size) { return SetSize(Y, size); }
    void ClearSize(Axis axis);

    void ClearConstraints();

    void SetSizeDefault(Axis axis, float size);
    void SetWidthDefault(float size) { return SetSizeDefault(X, size); }
    void SetHeightDefault(float size) { return SetSizeDefault(Y, size); }

    void SetParent(Layout *layout);

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

    virtual void RenderElement(Renderer *renderer) const { };
    virtual void RenderElementPost(Renderer *renderer) const { };

    // make sure you call these down if you override them
    bool EventHookedIs(const EventTypeBase &event) const;
        
    virtual void EventAttached(const EventTypeBase *id);
    virtual void EventDetached(const EventTypeBase *id);
    
    // Lua
    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, GetStaticType()); } // see Layout::luaF_Register for what yours should look like
    void luaF_RegisterWorker(lua_State *L, const char *name) const;

    static void luaF_RegisterFunctions(lua_State *L);

    static void luaF_RegisterFunction(lua_State *L, const char *owner, const char *name, int (*func)(lua_State *));

  private:
    void Render(Renderer *renderer) const;

    // Layout engine
    void Invalidate(Axis axis);
    void Obliterate_Detach(); // Detach this layout from all layouts
    void Obliterate_Extract();  // Detach everything that refers to this layout
    void Obliterate_Extract_Axis(Axis axis);  // Detach everything that refers to this axis
    void Obliterate_Extract_From(Axis axis, const Layout *layout);
    void Resolve();
    struct AxisData {
      AxisData() : size_cached(Utility::Undefined), size_set(Utility::Undefined), size_default(40) { };

      mutable float size_cached;

      struct Connector {
        Connector() : link(0), point_mine(Utility::Undefined), point_link(Utility::Undefined), offset(Utility::Undefined), cached(Utility::Undefined) { };

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
    const char *m_name_static;
    int m_name_id;
    std::string m_name_dynamic;
    
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
    #define FRAMES_LAYOUT_ASSERT(x, errstring, ...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, __VA_ARGS__))))
    #define FRAMES_LAYOUT_CHECK(x, errstring, ...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, __VA_ARGS__))))

    #define FRAMES_ERROR(...) GetEnvironment()->LogError(Utility::Format(__VA_ARGS__))
    #define FRAMES_DEBUG(...) GetEnvironment()->LogDebug(Utility::Format(__VA_ARGS__))

    #define CreateTagged(...) CreateTagged_imp(__FILE__, __LINE__, __VA_ARGS__)
  #else
    #define FRAMES_LAYOUT_ASSERT(x, errstring, args...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))
    #define FRAMES_LAYOUT_CHECK(x, errstring, args...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))

    #define FRAMES_ERROR(args...) GetEnvironment()->LogError(Utility::Format(args))
    #define FRAMES_DEBUG(args...) GetEnvironment()->LogDebug(Utility::Format(args))

    #define CreateTagged(args...) CreateTagged_imp(__FILE__, __LINE__, ## args)
  #endif
}

#include "frames/layout_template_inline.h"

#endif
