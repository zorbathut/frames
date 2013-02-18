// The Layout class

#ifndef FRAMES_LAYOUT
#define FRAMES_LAYOUT

#include "frames/delegate.h"
#include "frames/event_declaration.h"
#include "frames/input.h"
#include "frames/noncopyable.h"
#include "frames/point.h"
#include "frames/utility.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <set>
#include <map>

struct lua_State;

namespace Frames {
  class Environment;
  class Rect;
  class Renderer;
  class Layout;

  // This class is passed as a parameter to every event handler.
  // It's the method for requesting non-event-type-specific event information or modifying behavior about the event.
  class EventHandle {
  public:
    EventHandle(const EventHandle &ev);

    Layout *GetTarget() const { return m_target; }

    // Enabled when it's exited Sink mode, disabled again when entering bubble.
    // If triggered, prevents future events for this message.
    bool CanFinalize() const { return m_finalize_can; }
    void Finalize();
    bool GetFinalize() const { return m_finalize; }

    // INTERNAL ONLY BELOW THIS LINE, we'll improve this later
    static EventHandle INTERNAL_Initialize(Layout *layout);
    void INTERNAL_SetCanFinalize(bool finalizeable) { m_finalize_can = finalizeable; }
    static void INTERNAL_l_CreateMetatable(lua_State *L);
  private:
    EventHandle();

    Layout *m_target;

    bool m_finalize;
    bool m_finalize_can;

    static int l_GetTarget(lua_State *L);
    static int l_CanFinalize(lua_State *L);
    static int l_Finalize(lua_State *L);
    static int l_GetFinalize(lua_State *L);
  };

  template<typename F> struct LayoutHandlerMunger;
  template<> struct LayoutHandlerMunger<void ()> { typedef int (*T)(lua_State *); };
  template<typename P1> struct LayoutHandlerMunger<void (P1)> { typedef int (*T)(lua_State *, P1); };
  template<typename P1, typename P2> struct LayoutHandlerMunger<void (P1, P2)> { typedef int (*T)(lua_State *, P1, P2); };
  template<typename P1, typename P2, typename P3> struct LayoutHandlerMunger<void (P1, P2, P3)> { typedef int (*T)(lua_State *, P1, P2, P3); };
  template<typename P1, typename P2, typename P3, typename P4> struct LayoutHandlerMunger<void (P1, P2, P3, P4)> { typedef int (*T)(lua_State *, P1, P2, P3, P4); };

  typedef intptr_t EventId;

  class Layout : Noncopyable {
  private:
    friend class Environment;
    template<typename> friend class INTERNAL_EventHandlerCaller;
    template<typename> friend class INTERNAL_EventDispatch;
    template<typename> friend class INTERNAL_EventDispatchBubble;

    struct LuaFrameEventHandler;
    struct Sorter { bool operator()(const Layout *lhs, const Layout *rhs) const; };

  protected:
    class EventHandler;

  public:
    typedef std::multimap<float, EventHandler> EventMap;

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

    Layout *GetFrameUnder(int x, int y);

    // RetrieveHeight/RetrieveWidth/RetrievePoint/etc?
    // Events?

    FRAMES_FRAMEEVENT_DECLARE(Move, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE(Size, (), (EventHandle *event));

    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseOver, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMove, (const Point &pt), (EventHandle *event, const Point &pt));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMoveOutside, (const Point &pt), (EventHandle *event, const Point &pt));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseOut, (), (EventHandle *event));

    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseLeftUp, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseLeftUpOutside, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseLeftDown, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseLeftClick, (), (EventHandle *event));

    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMiddleUp, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMiddleUpOutside, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMiddleDown, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseMiddleClick, (), (EventHandle *event));

    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseRightUp, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseRightUpOutside, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseRightDown, (), (EventHandle *event));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseRightClick, (), (EventHandle *event));

    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseButtonUp, (int button), (EventHandle *event, int button));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseButtonUpOutside, (int button), (EventHandle *event, int button));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseButtonDown, (int button), (EventHandle *event, int button));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseButtonClick, (int button), (EventHandle *event, int button));

    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(MouseWheel, (int delta), (EventHandle *event, int delta));

    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(KeyDown, (const KeyEvent &kev), (EventHandle *event, const KeyEvent &kev));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(KeyType, (const std::string &text), (EventHandle *event, const std::string &text));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(KeyRepeat, (const KeyEvent &kev), (EventHandle *event, const KeyEvent &kev));
    FRAMES_FRAMEEVENT_DECLARE_BUBBLE(KeyUp, (const KeyEvent &kev), (EventHandle *event, const KeyEvent &kev));

    const char *GetNameStatic() const { return m_name_static; }
    void SetNameStatic(const char *name) { m_name_static = name; }  // WARNING: This does not make a copy! The const char* must have a lifetime longer than this frame.

    int GetNameId() const { return m_name_id; }
    void SetNameId(int id) { m_name_id = id; }

    const std::string &GetNameDynamic() const { return m_name_dynamic; }
    void SetNameDynamic(const std::string &name) { m_name_dynamic = name; }

    typedef std::set<Layout *, Sorter> ChildrenList;
    const ChildrenList &GetChildren() { return m_children; }

    Environment *GetEnvironment() const { return m_env; }

    // Lua-specific
    void l_push(lua_State *L) const;

    std::string GetName() const;
    std::string GetNameFull() const;    // THIS MIGHT BE VERY, VERY SLOW

    // Debug
    void DebugDumpLayout() const;

  protected:
    Layout(Layout *parent, Environment *env = 0);
    virtual ~Layout();

    // while Layout isn't mutable, things that inherit from Layout might be
    void SetPoint(Axis axis, float mypt, const Layout *link, float theirpt, float offset);
    void ClearPoint(Axis axis, float mypt);
    void ClearAllPoints(Axis axis);

    void SetSize(Axis axis, float size);
    void SetWidth(float size) { return SetSize(X, size); }
    void SetHeight(float size) { return SetSize(Y, size); }
    void ClearSize(Axis axis);

    void ClearLayout();

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
    virtual bool TestMouseMasking(int x, int y) { return true; }

    void Obliterate(); // prep for destruction along with all children

    virtual void RenderElement(Renderer *renderer) const { };
    virtual void RenderElementPost(Renderer *renderer) const { };

    // Event-related
    bool HasEvent(EventId id) const;
    
    // make sure you call these down if you override them
    virtual void EventAttached(EventId id);
    virtual void EventDetached(EventId id);

    // Lua
    virtual void l_Register(lua_State *L) const { l_RegisterWorker(L, GetStaticType()); } // see Layout::l_Register for what yours should look like
    void l_RegisterWorker(lua_State *L, const char *name) const;

    static void l_RegisterFunctions(lua_State *L);

    static void l_RegisterFunction(lua_State *L, const char *owner, const char *name, int (*func)(lua_State *));
    template<typename Prototype> static void l_RegisterEvent(lua_State *L, const char *owner, const char *nameAttach, const char *nameDetach, EventId eventId, typename LayoutHandlerMunger<Prototype>::T pusher);

    template<typename Prototype> static int l_RegisterEventAttach(lua_State *L);
    template<typename Prototype> static int l_RegisterEventDetach(lua_State *L);

    void l_ClearLuaEvents(lua_State *lua);
    void l_ClearLuaEvents_Recursive(lua_State *lua);

    static int l_EventPusher_Default(lua_State *L);
    static int l_EventPusher_Default(lua_State *L, int p1);
    static int l_EventPusher_Default(lua_State *L, const std::string &pt);
    static int l_EventPusher_Default(lua_State *L, const Point &pt);
    static int l_EventPusher_Default(lua_State *L, const KeyEvent &pt);

    static int l_EventPusher_Button(lua_State *L, int button);

    // Various internal-only functionality
    void INTERNAL_EventAttach(EventId id, const EventHandler &handler, float order);
    bool INTERNAL_EventDetach(EventId id, const EventHandler &handler, float order);

    class EventHandler {
    public:
      // NOTE: this works because delegate is POD
      EventHandler() { }
      template<typename T> EventHandler(Delegate<T> din) : m_type(TYPE_NATIVE), m_lua(0) {
        typedef Delegate<T> dintype;
        BOOST_STATIC_ASSERT(sizeof(dintype) == sizeof(Delegate<void ()>));

        *reinterpret_cast<dintype *>(c.m_delegate) = din;
      }
      template<typename T> EventHandler(Delegate<T> din, const LuaFrameEventHandler *lua) : m_type(TYPE_LUA), m_lua(lua) {
        typedef Delegate<T> dintype;
        BOOST_STATIC_ASSERT(sizeof(dintype) == sizeof(Delegate<void ()>));

        *reinterpret_cast<dintype *>(c.m_delegate) = din;
      }
      ~EventHandler() { }

      template<typename T1> void Call(T1 t1) const {
        if (!IsErased()) (*reinterpret_cast<const Delegate<void (T1)> *>(c.m_delegate))(t1);
      }

      template<typename T1, typename T2> void Call(T1 t1, T2 t2) const {
        if (!IsErased()) (*reinterpret_cast<const Delegate<void (T1, T2)> *>(c.m_delegate))(t1, t2);
      }

      template<typename T1, typename T2, typename T3> void Call(T1 t1, T2 t2, T3 t3) const {
        if (!IsErased()) (*reinterpret_cast<const Delegate<void (T1, T2, T3)> *>(c.m_delegate))(t1, t2, t3);
      }

      template<typename T1, typename T2, typename T3, typename T4> void Call(T1 t1, T2 t2, T3 t3, T4 t4) const {
        if (!IsErased()) (*reinterpret_cast<const Delegate<void (T1, T2, T3, T4)> *>(c.m_delegate))(t1, t2, t3, t4);
      }

      bool IsNative() const { return m_type == TYPE_NATIVE; }

      bool IsLua() const { return m_type == TYPE_LUA; }
      const LuaFrameEventHandler *GetLua() const { return m_lua; }

      bool IsErased() const { return m_type == TYPE_ERASED; }
      void MarkErased() { /* TODO: make sure it is not yet erased */ m_type = TYPE_ERASED; m_lua = 0; }

    private:
      union {
        char m_delegate[sizeof(Delegate<void ()>)];
      } c;

      enum { TYPE_ERASED, TYPE_NATIVE, TYPE_LUA } m_type;

      union {
        const LuaFrameEventHandler *m_lua;
      };
      
      friend bool operator==(const EventHandler &lhs, const EventHandler &rhs);
    };

  private:
    friend bool operator==(const EventHandler &lhs, const EventHandler &rhs);
    friend struct EventhandlerInfo;

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
    std::map<EventId, EventMap> m_events;
    bool m_event_locked;
    bool m_event_buffered;
    bool Event_Lock(); // must store the result of this to pass back into Unlock
    void Event_Unlock(bool original);

    // Obliterate buffering (sort of related to the event system)
    bool m_obliterate_locked;
    bool m_obliterate_buffered;
    bool Obliterate_Lock(); // must store the result of this to pass back into Unlock
    void Obliterate_Unlock(bool original); // note that, after calling this function, the frame might eat itself

    // Global environment
    Environment *m_env;

    // Lua frame event system
    struct LuaFrameEventHandler {
      LuaFrameEventHandler(lua_State *L, int idx, Layout *layout, Utility::intfptr_t pusher) : L(L), layout(layout), pusher(pusher), idx(idx) { };
      lua_State *L;
      Layout *layout; // we need this so we can push the right frame in
      Utility::intfptr_t pusher; // this is cast to whatever other function type may be necessary
      int idx;
      
      void Call(EventHandle *handle) const;
      void Call(EventHandle *handle, int p1) const;
      void Call(EventHandle *handle, const std::string &text) const;
      void Call(EventHandle *handle, const Point &pt) const;
      void Call(EventHandle *handle, const KeyEvent &ke) const;

    private:
      void CallSetup(lua_State *L, EventHandle *eh) const;
      void CallTeardown(lua_State *L) const;
    };
    friend bool operator<(const LuaFrameEventHandler &lhs, const LuaFrameEventHandler &rhs);
    typedef std::map<LuaFrameEventHandler, int> LuaFrameEventMap; // second parameter is refcount
    template<typename Prototype> void l_EventAttach(lua_State *L, EventId event, int idx, float priority, typename LayoutHandlerMunger<Prototype>::T pusher);
    template<typename Prototype> bool l_EventDetach(lua_State *L, EventId event, int idx, float priority);
    bool l_EventDetach(lua_State *L, LuaFrameEventMap::iterator itr, EventId event, EventHandler handler, float priority);
    LuaFrameEventMap m_lua_events;
    // NOTE: refcount is just for our internal refcounting! we need to change the global count every time this goes up or down

    // Lua bindings
    static int l_GetLeft(lua_State *L);
    static int l_GetRight(lua_State *L);
    static int l_GetTop(lua_State *L);
    static int l_GetBottom(lua_State *L);
    static int l_GetBounds(lua_State *L);

    static int l_GetWidth(lua_State *L);
    static int l_GetHeight(lua_State *L);

    static int l_GetChildren(lua_State *L);

    static int l_GetName(lua_State *L);
    static int l_GetNameFull(lua_State *L);
    static int l_GetType(lua_State *L);

    static int l_DebugDumpLayout(lua_State *L);
  };

  // Debug code
  #define FRAMES_LAYOUT_ASSERT(x, errstring, args...) (__builtin_expect(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))
  #define FRAMES_LAYOUT_CHECK(x, errstring, args...) (__builtin_expect(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))

  #define FRAMES_ERROR(args...) GetEnvironment()->LogError(Utility::Format(args))
  #define FRAMES_DEBUG(args...) GetEnvironment()->LogDebug(Utility::Format(args))

  #define CreateTagged(args...) CreateTagged_imp(__FILE__, __LINE__, ## args)
}

#endif
