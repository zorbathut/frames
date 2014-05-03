// The Text class

#ifndef FRAMES_TEXT
#define FRAMES_TEXT

#include "frames/frame.h"

#include "frames/text_manager.h"

namespace Frames {
  class Text : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    enum InteractivityMode { INTERACTIVE_NONE, INTERACTIVE_SELECT, INTERACTIVE_CURSOR, INTERACTIVE_EDIT };

    static Text *Create(const std::string &name, Layout *parent);

    void TextSet(const std::string &text);
    const std::string &TextGet() const { return m_text; }

    void FontSet(const std::string &id);
    const std::string &GetFont() const { return m_font; }

    void SizeSet(float size);
    float SizeGet() const { return m_size; }

    void WordwrapSet(bool wordwrap);
    bool WordwrapGet() const { return m_wordwrap; }

    void ColorSet(const Color &color);
    const Color &ColorGet() const { return m_color_text; }

    void InteractiveSet(InteractivityMode interactive);
    InteractivityMode InteractiveGet() const { return m_interactive; }

    void CursorSet(int position);
    int CursorGet() const { return m_cursor; }

    void SelectionClear();  // clear
    void SelectionSet(int start, int end);
    bool SelectionGet(int *start, int *end) const;

    void ScrollSet(const Vector &scroll);
    const Vector &ScrollGet() const { return m_scroll; }

    void ColorSelectionSet(const Color &color);
    const Color &ColorSelectionGet() const { return m_color_selection; }

    void ColorSelectedSet(const Color &color);
    const Color &ColorSelectedGet() const { return m_color_selected; }

  protected:
    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, TypeStaticGet()); Frame::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Text(const std::string &name, Layout *parent);
    virtual ~Text();
  
    void SizeChanged(Handle *handle);
    void UpdateLayout();
    void ScrollToCursor();

    virtual void RenderElement(detail::Renderer *renderer) const;

    std::string m_font;
    std::string m_text;
    float m_size;
    bool m_wordwrap;
    detail::TextLayoutPtr m_layout;

    Color m_color_text;
    Color m_color_selection;
    Color m_color_selected;

    InteractivityMode m_interactive;

    Vector m_scroll;

    int m_select;
    int m_cursor;

    // Event handlers for mouse events
    void EventInternal_LeftDown(Handle *e);
    void EventInternal_LeftUp(Handle *e);
    void EventInternal_Move(Handle *e, const Vector &pt);

    // Event handlers for key events
    void EventInternal_KeyText(Handle *e, const std::string &type);
    void EventInternal_KeyDownOrRepeat(Handle *e, Input::Key key);

    // Lua bindings
    static int luaF_SetText(lua_State *L);
    static int luaF_GetText(lua_State *L);

    static int luaF_SetFont(lua_State *L);
    static int luaF_GetFont(lua_State *L);

    static int luaF_SizeSet(lua_State *L);
    static int luaF_SizeGet(lua_State *L);

    static int luaF_SetWordwrap(lua_State *L);
    static int luaF_GetWordwrap(lua_State *L);

    static int luaF_SetColor(lua_State *L);
    static int luaF_GetColor(lua_State *L);

    static int luaF_SetInteractive(lua_State *L);
    static int luaF_GetInteractive(lua_State *L);

    static int luaF_SetCursor(lua_State *L);
    static int luaF_GetCursor(lua_State *L);

    static int luaF_SetSelection(lua_State *L);
    static int luaF_GetSelection(lua_State *L);

    static int luaF_SetScroll(lua_State *L);
    static int luaF_GetScroll(lua_State *L);

    static int luaF_SetColorSelection(lua_State *L);
    static int luaF_GetColorSelection(lua_State *L);

    static int luaF_SetColorSelected(lua_State *L);
    static int luaF_GetColorSelected(lua_State *L);
  };
}

#endif
