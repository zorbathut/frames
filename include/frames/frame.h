// The Frame class

#ifndef FRAMES_FRAME
#define FRAMES_FRAME

#include "frames/layout.h"

namespace Frames {
  class Frame : public Layout {
    friend class Environment;

  public:
    static Frame *CreateBare(Layout *parent);
    static Frame *CreateTagged_imp(const char *filename, int line, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }

    using Layout::SetPoint;

    using Layout::SetSize;
    using Layout::SetWidth;
    using Layout::SetHeight;

    using Layout::SetParent;
    using Layout::GetParent;

    using Layout::SetLayer;
    using Layout::GetLayer;

    using Layout::SetStrata;
    using Layout::GetStrata;

    using Layout::SetVisible;
    using Layout::GetVisible;

    void SetBackground(float r, float g, float b, float a = 1.0f);
    void GetBackground(float *r, float *g, float *b, float *a) const;

    using Layout::Obliterate;

  protected:
    Frame(Layout *parent);
    virtual ~Frame();

    virtual void RenderElement(Renderer *renderer) const;

    void l_Register(lua_State *L) const { l_RegisterWorker(L, GetStaticType()); Layout::l_Register(L); }

    static void l_RegisterFunctions(lua_State *L);

  private:
    float m_bg_r, m_bg_g, m_bg_b, m_bg_a;

    // Lua bindings
    static int l_SetPoint(lua_State *L);

    static int l_SetWidth(lua_State *L);
    static int l_SetHeight(lua_State *L);

    static int l_SetParent(lua_State *L);
    static int l_GetParent(lua_State *L);

    static int l_SetLayer(lua_State *L);
    static int l_GetLayer(lua_State *L);

    static int l_SetStrata(lua_State *L);
    static int l_GetStrata(lua_State *L);

    static int l_SetVisible(lua_State *L);
    static int l_GetVisible(lua_State *L);

    static int l_SetBackground(lua_State *L);
    static int l_GetBackground(lua_State *L);

    static int l_Obliterate(lua_State *L);
  };
}

#endif
