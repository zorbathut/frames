// The Layout class

#ifndef FRAMES_LAYOUT
#define FRAMES_LAYOUT

#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/utility.h"

#include <vector>
#include <set>

namespace Frames {
  enum Axis { X, Y }; // axes

  class Layout : Noncopyable {
  public:
    float GetAxis(Axis axis, float pt) const;
    float GetLeft() const { return GetAxis(X, 0); }
    float GetRight() const { return GetAxis(X, 1); }
    float GetTop() const { return GetAxis(Y, 0); }
    float GetBottom() const { return GetAxis(Y, 1); }

    float GetSize(Axis axis) const;
    float GetWidth() const { return GetSize(X); }
    float GetHeight() const { return GetSize(Y); }

    // GetType?
    // RetrieveHeight/RetrieveWidth/RetrievePoint/etc?
    // ClearHeight/ClearWidth/ClearPoint/etc?
    // Events?

    Environment *GetEnvironment() const;

  protected:
    Layout(Environment *env);
    ~Layout();

    // while Layout isn't mutable, things that inherit from Layout might be
   void SetPoint(Axis axis, float mypt, LayoutPtr target, float theirpt, float offset);

   void SetSize(Axis axis, float size);
   void SetWidth(float size) { return SetSize(X, size); }
   void SetHeight(float size) { return SetSize(Y, size); }

   void SetSizeDefault(Axis axis, float size);
   void SetWidthDefault(float size) { return SetSizeDefault(X, size); }
   void SetHeightDefault(float size) { return SetSizeDefault(Y, size); }

   void SetParent(LayoutPtr layout);
   LayoutPtr GetParent() const { return m_parent; }

   void SetLayer(float layer);
   float GetLayer() const { return m_layer; }

   void SetStrata(float strata);
   float GetStrata() const { return m_strata; }

   void SetVisible(bool visible);
   bool GetVisible() const { return m_visible; }

  private:
    friend class Environment;

    void Render();
    virtual void RenderElement() { };

    // Layout engine
    struct AxisData {
      AxisData() : size_cached(Utility::NaN), size_set(Utility::NaN), size_default(40) { };

      float size_cached;

      struct Connector {
        Connector() : point_src(Utility::NaN), point_dst(Utility::NaN), offset(Utility::NaN), cached(Utility::NaN) { };

        LayoutPtr link;
        float point_src;
        float point_dst;
        float offset;

        float cached;
      };
      Connector connections[2];

      float size_set;
      float size_default;

      std::multiset<LayoutPtr> children;
    };
    AxisData m_axes[2];
    bool m_resolved;  // whether *this* frame has its layout completely determined
    bool m_resolved_recursively;  // whether this frame and all its children have their layouts completely determined

    // Layer/parenting engine
    float m_layer;
    float m_strata;
    LayoutPtr m_parent;
    std::set<LayoutPtr> m_children;
    std::vector<LayoutPtr> m_children_ordered;
    bool m_visible;

    Environment *m_env;
  };

  // implementation detail for LayoutPtr and the like, do not call directly!
  void intrusive_ptr_add_ref(Frames::Layout *layout);
  void intrusive_ptr_release(Frames::Layout *layout);
}

#endif
