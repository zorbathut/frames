// The Frame class

#ifndef FRAMES_FRAME
#define FRAMES_FRAME

#include "frames/color.h"
#include "frames/layout.h"

namespace Frames {
  class Frame : public Layout {
    friend class Environment;

  public:
    static Frame *Create(const std::string &name, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }

    using Layout::SetPoint;

    using Layout::ClearPoint;
    using Layout::ClearAllPoints;

    using Layout::SetSize;
    using Layout::SetWidth;
    using Layout::SetHeight;
    using Layout::ClearSize;

    using Layout::ClearConstraints;

    using Layout::SetParent;
    using Layout::GetParent;

    using Layout::SetLayer;
    using Layout::GetLayer;

    using Layout::SetImplementation;
    using Layout::GetImplementation;

    using Layout::SetVisible;
    using Layout::GetVisible;

    using Layout::SetAlpha;
    using Layout::GetAlpha;

    void SetBackground(const Color &color);
    const Color &GetBackground() const { return m_bg; }

    void SetFocus(bool focus);
    bool GetFocus() const;

    using Layout::Obliterate;

  protected:
    Frame(const std::string &name, Layout *parent);
    virtual ~Frame();

    virtual void RenderElement(detail::Renderer *renderer) const;

    void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, GetStaticType()); Layout::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Color m_bg;

    // Lua bindings
    static int luaF_SetPoint(lua_State *L);

    static int luaF_SetWidth(lua_State *L);
    static int luaF_SetHeight(lua_State *L);

    static int luaF_SetParent(lua_State *L);
    static int luaF_GetParent(lua_State *L);

    static int luaF_SetLayer(lua_State *L);
    static int luaF_GetLayer(lua_State *L);

    static int luaF_SetImplementation(lua_State *L);
    static int luaF_GetImplementation(lua_State *L);

    static int luaF_SetVisible(lua_State *L);
    static int luaF_GetVisible(lua_State *L);

    static int luaF_SetAlpha(lua_State *L);
    static int luaF_GetAlpha(lua_State *L);

    static int luaF_SetBackground(lua_State *L);
    static int luaF_GetBackground(lua_State *L);

    static int luaF_SetFocus(lua_State *L);
    static int luaF_GetFocus(lua_State *L);

    static int luaF_Obliterate(lua_State *L);
  };
}

#endif
