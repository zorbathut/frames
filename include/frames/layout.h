// The Layout class

#ifndef FRAMES_LAYOUT
#define FRAMES_LAYOUT

#include "frames/delegate.h"
#include "frames/noncopyable.h"
#include "frames/utility.h"

#include <vector>
#include <set>
#include <map>

struct lua_State;

namespace Frames {
  class Environment;
  class Rect;
  class Renderer;

  class EventHandle {
  };

  // we'll add more internal utility functions if/when needed
  #define FRAMES_LAYOUT_EVENT_HOOKS(eventname, paramlist, paramlistcomplete) \
      void Event##eventname##Attach(Delegate<void paramlistcomplete> delegate, float order = 0.f); \
      void Event##eventname##Detach(Delegate<void paramlistcomplete> delegate, float order = 0.f / 0.f); \
      static intptr_t Event##eventname##Id(); \
      typedef void Event##eventname##Functiontype paramlistcomplete; \
    private: \
      static char s_event_##eventname##_id; /* the char itself isn't the ID, the address of the char is */ \
    public:

  #define FRAMES_LAYOUT_EVENT_DECLARE(eventname, paramlist, paramlistcomplete) \
      FRAMES_LAYOUT_EVENT_HOOKS(eventname, paramlist, paramlistcomplete) \
    private: \
      void Event##eventname##Trigger paramlist const; \
    public:

  #define FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(eventname, paramlist, paramlistcomplete) \
      FRAMES_LAYOUT_EVENT_HOOKS(eventname, paramlist, paramlistcomplete) \
      FRAMES_LAYOUT_EVENT_HOOKS(eventname##Sink, paramlist, paramlistcomplete) \
      FRAMES_LAYOUT_EVENT_HOOKS(eventname##Bubble, paramlist, paramlistcomplete) \
    private: \
      void Event##eventname##Trigger paramlist const; \
    public:

  class Layout : Noncopyable {
    friend class Environment;

    class EventHandler; // defined in layout.cpp
    struct Sorter { bool operator()(const Layout *lhs, const Layout *rhs) const; };

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

    FRAMES_LAYOUT_EVENT_DECLARE(Move, (), (EventHandle *event));
    FRAMES_LAYOUT_EVENT_DECLARE(Size, (), (EventHandle *event));

    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseOver, (), (EventHandle *event));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseOut, (), (EventHandle *event));

    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseLeftUp, (), (EventHandle *event));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseLeftDown, (), (EventHandle *event));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseLeftClick, (), (EventHandle *event));

    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseMiddleUp, (), (EventHandle *event));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseMiddleDown, (), (EventHandle *event));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseMiddleClick, (), (EventHandle *event));

    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseRightUp, (), (EventHandle *event));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseRightDown, (), (EventHandle *event));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseRightClick, (), (EventHandle *event));

    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseButtonUp, (int button), (EventHandle *event, int button));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseButtonDown, (int button), (EventHandle *event, int button));
    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseButtonClick, (int button), (EventHandle *event, int button));

    FRAMES_LAYOUT_EVENT_DECLARE_BUBBLE(MouseWheel, (int delta), (EventHandle *event, int delta));

    const char *GetNameStatic() const { return m_name_static; }
    void SetNameStatic(const char *name) { m_name_static = name; }  // WARNING: This does not make a copy! The const char* must have a lifetime longer than this frame.

    int GetNameId() const { return m_name_id; }
    void SetNameId(int id) { m_name_id = id; }

    const std::string &GetNameDynamic() const { return m_name_dynamic; }
    void SetNameStatic(const std::string &name) { m_name_dynamic = name; }

    typedef std::set<Layout *, Sorter> ChildrenList;
    const ChildrenList &GetChildren() { return m_children; }

    Environment *GetEnvironment() const { return m_env; }

    // Lua-specific
    void l_push(lua_State *L) const;

    std::string GetName() const;
    std::string GetNameFull() const;    // THIS MIGHT BE VERY, VERY SLOW

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

    // This is for further-down classes, not so useful for users
    void SetFullMouseMasking(bool mask) { m_fullMouseMasking = mask; }
    bool GetFullMouseMasking() { return m_fullMouseMasking; }
    virtual bool TestMouseMasking(int x, int y) { return true; }

    void Obliterate(); // prep for destruction along with all children

    virtual void RenderElement(Renderer *renderer) const { };
    virtual void RenderElementPost(Renderer *renderer) const { };

    // Event-related
    const EventMap *GetEventTable(intptr_t id) const; // returns null on no events
    
    // make sure you call these down if you override them
    virtual void EventAttached(intptr_t id);
    virtual void EventDetached(intptr_t id);

    // Lua
    virtual void l_Register(lua_State *L) const { l_RegisterWorker(L, GetStaticType()); } // see Layout::l_Register for what yours should look like
    void l_RegisterWorker(lua_State *L, const char *name) const;

    static void l_RegisterFunctions(lua_State *L);

    static void l_RegisterFunction(lua_State *L, const char *owner, const char *name, int (*func)(lua_State *));
    template<typename Prototype> static void l_RegisterEvent(lua_State *L, const char *owner, const char *nameAttach, const char *nameDetach, intptr_t eventId);

    template<typename Prototype> static int l_RegisterEventAttach(lua_State *L);
    template<typename Prototype> static int l_RegisterEventDetach(lua_State *L);

    void l_ClearLuaEvents(lua_State *lua);
    void l_ClearLuaEvents_Recursive(lua_State *lua);

    // Various internal-only functionality
    void EventAttach(intptr_t id, const EventHandler &handler, float order);
    bool EventDetach(intptr_t id, const EventHandler &handler, float order);

  private:
    friend bool operator==(const EventHandler &lhs, const EventHandler &rhs);
    friend struct EventhandlerInfo;

    void Render(Renderer *renderer) const;

    // Layout engine
    void Invalidate(Axis axis) const;
    void Obliterate_Detach(); // Detach this layout from all layouts
    void Obliterate_Extract();  // Detach everything that refers to this layout
    void Obliterate_Extract_Axis(Axis axis);  // Detach everything that refers to this axis
    void Obliterate_Extract_From(Axis axis, const Layout *layout);
    void Resolve() const;
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

    // Input
    bool m_fullMouseMasking;
    bool m_acceptInput;

    // Naming system
    const char *m_name_static;
    int m_name_id;
    std::string m_name_dynamic;

    // Event system
    std::map<intptr_t, EventMap> m_events;

    // Global environment
    Environment *m_env;

    // Lua frame event system
    struct LuaFrameEventHandler {
      LuaFrameEventHandler(lua_State *L, int idx, Layout *layout) : L(L), idx(idx), layout(layout) { };
      lua_State *L;
      int idx;
      Layout *layout; // we need this so we can push the right frame in

      void Call(EventHandle *handle) const;
      void Call(EventHandle *handle, int p1) const;
    };
    friend bool operator<(const LuaFrameEventHandler &lhs, const LuaFrameEventHandler &rhs);
    typedef std::map<LuaFrameEventHandler, int> LuaFrameEventMap; // second parameter is refcount
    template<typename Prototype> void l_EventAttach(lua_State *L, intptr_t event, int idx, float priority);
    template<typename Prototype> bool l_EventDetach(lua_State *L, intptr_t event, int idx, float priority);
    bool l_EventDetach(lua_State *L, LuaFrameEventMap::iterator itr, intptr_t event, EventHandler handler, float priority);
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
  };

  // Debug code
  #define FRAMES_LAYOUT_ASSERT(x, errstring, args...) (__builtin_expect(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))
  #define FRAMES_LAYOUT_CHECK(x, errstring, args...) (__builtin_expect(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))

  #define FRAMES_ERROR(args...) GetEnvironment()->LogError(Utility::Format(args))
  #define FRAMES_DEBUG(args...) GetEnvironment()->LogDebug(Utility::Format(args))

  #define CreateTagged(args...) CreateTagged_imp(__FILE__, __LINE__, ## args)
}

#endif
