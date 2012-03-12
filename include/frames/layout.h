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

  // we'll add more internal utility functions if/when needed
  #define FRAMES_LAYOUT_EVENT_DECLARE(eventname, paramlist) \
    void Event##eventname##Attach(Delegate<void paramlist> delegate, float order = 0.f); \
    void Event##eventname##Detach(Delegate<void paramlist> delegate); \
    private: \
    void Event##eventname##Trigger paramlist const; \
    static char s_event_##eventname##_id; /* the char itself isn't the ID, the address of the char is */ \
    public:

  class Layout : Noncopyable {
    class EventHandler; // defined in layout.cpp
    struct Sorter { bool operator()(const Layout *lhs, const Layout *rhs) const; };

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

    // RetrieveHeight/RetrieveWidth/RetrievePoint/etc?
    // ClearHeight/ClearWidth/ClearPoint/etc?
    // Events?

    FRAMES_LAYOUT_EVENT_DECLARE(Move, ());
    FRAMES_LAYOUT_EVENT_DECLARE(Size, ());

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

    // THIS MIGHT BE VERY, VERY SLOW
    std::string GetNameDebug() const;

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
    Layout *GetParent() const { return m_parent; }

    void SetLayer(float layer);
    float GetLayer() const { return m_layer; }

    void SetStrata(float strata);
    float GetStrata() const { return m_strata; }

    void SetVisible(bool visible);
    bool GetVisible() const { return m_visible; }

    // This should be called only by the general-purpose macros
    void EventAttach(unsigned int id, const EventHandler &handler, float order);
    void EventDetach(unsigned int id, const EventHandler &handler);

    void Obliterate(); // prep for destruction along with all children

    virtual void RenderElement(Renderer *renderer) const { };
    virtual void RenderElementPost(Renderer *renderer) const { };

    // Lua
    void l_Register(lua_State *L) const { l_RegisterWorker(L, GetStaticType()); } // see Layout::l_Register for what yours should look like
    void l_RegisterWorker(lua_State *L, const char *name) const;

  private:
    friend class Environment;
    friend bool operator==(const EventHandler &lhs, const EventHandler &rhs);

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
    bool m_obliterating;

    // Layout events
    mutable float m_last_width, m_last_height;
    mutable float m_last_x, m_last_y;

    // Layer/parenting engine
    float m_layer;
    float m_strata;
    Layout *m_parent;
    bool m_visible;
    ChildrenList m_children;

    // Naming system
    const char *m_name_static;
    int m_name_id;
    std::string m_name_dynamic;

    // Event system
    std::map<unsigned int, std::multimap<float, EventHandler> > m_events;

    // Global environment
    Environment *m_env;
  };

  // Debug code
  #define FRAMES_LAYOUT_ASSERT(x, errstring, args...) (__builtin_expect(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))
  #define FRAMES_LAYOUT_CHECK(x, errstring, args...) (__builtin_expect(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))

  #define FRAMES_ERROR(args...) GetEnvironment()->LogError(Utility::Format(args))
  #define FRAMES_DEBUG(args...) GetEnvironment()->LogDebug(Utility::Format(args))

  #define CreateTagged(args...) CreateTagged_imp(__FILE__, __LINE__, ## args)
}

#endif
