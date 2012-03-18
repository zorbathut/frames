// The Text class

#ifndef FRAMES_TEXT
#define FRAMES_TEXT

#include "frames/frame.h"

#include "frames/text_manager.h"

namespace Frames {
  class Text : public Frame {
    friend class Environment;

  public:
    static Text *CreateBare(Layout *parent);
    static Text *CreateTagged_imp(const char *filename, int line, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }

    void SetText(const std::string &text);
    const std::string &GetText() const { return m_text; }

    void SetFont(const std::string &id);
    const std::string &GetFont() const { return m_font; }

    void SetSize(float size);
    float GetSize() const { return m_size; }

    void SetWordwrap(bool wordwrap);
    bool GetWordwrap() const { return m_wordwrap; }

    void SetColor(const Color &color);
    const Color &GetColor() const { return m_color_text; }

  protected:
    virtual void l_Register(lua_State *L) const { l_RegisterWorker(L, GetStaticType()); Frame::l_Register(L); }

    static void l_RegisterFunctions(lua_State *L);

  private:
    Text(Layout *parent);
    virtual ~Text();
  
    void UpdateLayout();

    virtual void RenderElement(Renderer *renderer) const;

    std::string m_font;
    std::string m_text;
    float m_size;
    bool m_wordwrap;
    TextLayoutPtr m_layout;

    Color m_color_text;
    Color m_color_selection;
    Color m_color_selected;

    // Lua bindings
    static int l_SetText(lua_State *L);
    static int l_GetText(lua_State *L);

    static int l_SetFont(lua_State *L);
    static int l_GetFont(lua_State *L);

    static int l_SetSize(lua_State *L);
    static int l_GetSize(lua_State *L);

    static int l_SetWordwrap(lua_State *L);
    static int l_GetWordwrap(lua_State *L);

    static int l_SetColor(lua_State *L);
    static int l_GetColor(lua_State *L);
  };
}

#endif
