// The Layout class

#ifndef FRAMES_LAYOUT
#define FRAMES_LAYOUT

#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/utility.h"

#include <vector>
#include <set>

namespace Frames {
  class Environment;
  class Renderer;

  class Layout : Noncopyable {
  public:
    float GetPoint(Axis axis, float pt) const;
    float GetLeft() const { return GetPoint(X, 0); }
    float GetRight() const { return GetPoint(X, 1); }
    float GetTop() const { return GetPoint(Y, 0); }
    float GetBottom() const { return GetPoint(Y, 1); }

    float GetSize(Axis axis) const;
    float GetWidth() const { return GetSize(X); }
    float GetHeight() const { return GetSize(Y); }

    // GetType?
    // RetrieveHeight/RetrieveWidth/RetrievePoint/etc?
    // ClearHeight/ClearWidth/ClearPoint/etc?
    // Events?

    const char *GetNameStatic() const { return m_name_static; }
    void SetNameStatic(const char *name) { m_name_static = name; }  // WARNING: This does not make a copy! The const char* must have a lifetime longer than this frame.

    int GetNameId() const { return m_name_id; }
    void SetNameId(int id) { m_name_id = id; }

    const std::string &GetNameDynamic() const { return m_name_dynamic; }
    void SetNameStatic(const std::string &name) { m_name_dynamic = name; }

    Environment *GetEnvironment() const { return m_env; }

    // THIS MIGHT BE VERY, VERY SLOW
    std::string GetNameDebug() const;

  protected:
    Layout(const LayoutPtr &layout, Environment *env = 0);
    virtual ~Layout();

    // while Layout isn't mutable, things that inherit from Layout might be
   void SetPoint(Axis axis, float mypt, const LayoutPtr &link, float theirpt, float offset);
   void ClearPoint(Axis axis, float mypt);
   void ClearAllPoints(Axis axis);

   void SetSize(Axis axis, float size);
   void SetWidth(float size) { return SetSize(X, size); }
   void SetHeight(float size) { return SetSize(Y, size); }
   void ClearSize(Axis axis);

   void SetSizeDefault(Axis axis, float size);
   void SetWidthDefault(float size) { return SetSizeDefault(X, size); }
   void SetHeightDefault(float size) { return SetSizeDefault(Y, size); }

   void SetParent(const LayoutPtr &layout);
   LayoutPtr GetParent() const { return m_parent; }

   void SetLayer(float layer);
   float GetLayer() const { return m_layer; }

   void SetStrata(float strata);
   float GetStrata() const { return m_strata; }

   void SetVisible(bool visible);
   bool GetVisible() const { return m_visible; }

  private:
    friend class Environment;

    void Render(Renderer *renderer);
    virtual void RenderElement(Renderer *renderer) { };

    // Layout engine
    void Invalidate(Axis axis) const;
    void Resolve() const;
    struct AxisData {
      AxisData() : size_cached(Utility::Undefined), size_set(Utility::Undefined), size_default(40) { };

      mutable float size_cached;

      struct Connector {
        Connector() : point_mine(Utility::Undefined), point_link(Utility::Undefined), offset(Utility::Undefined), cached(Utility::Undefined) { };

        LayoutPtr link;
        float point_mine;
        float point_link;
        float offset;

        mutable float cached;
      };
      Connector connections[2];

      float size_set;
      float size_default;

      typedef std::multiset<LayoutPtr> ChildrenList;
      ChildrenList children;
    };
    AxisData m_axes[2];
    mutable bool m_resolved;  // whether *this* frame has its layout completely determined

    // Layer/parenting engine
    struct Sorter { bool operator()(const LayoutPtr &lhs, const LayoutPtr &rhs) const; };
    float m_layer;
    float m_strata;
    LayoutPtr m_parent;
    bool m_visible;
    typedef std::set<LayoutPtr, Sorter> ChildrenList;
    ChildrenList m_children;

    // Naming system
    const char *m_name_static;
    int m_name_id;
    std::string m_name_dynamic;

    Environment *m_env;
  };

  // Debug code
  #define FRAMES_LAYOUT_ASSERT(x, errstring, args...) (__builtin_expect(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))
  #define FRAMES_LAYOUT_CHECK(x, errstring, args...) (__builtin_expect(!!(x), 1) ? (void)(1) : (GetEnvironment()->LogError(Utility::Format(errstring, ## args))))

  #define FRAMES_DEBUG(args...) GetEnvironment()->LogDebug(Utility::Format(args))

  #define CreateTagged(args...) CreateTagged_imp(__FILE__, __LINE__, ## args)
}

#endif
