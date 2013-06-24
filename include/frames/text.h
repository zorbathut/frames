// The Text class

#ifndef FRAME_TEXT
#define FRAME_TEXT

#include "frames/frame.h"

#include "frames/text_manager.h"

namespace Frame {
  class Text : public Frame {
    friend class Environment;

  public:
    enum InteractivityMode { INTERACTIVE_NONE, INTERACTIVE_SELECT, INTERACTIVE_CURSOR, INTERACTIVE_EDIT };

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

    void SetInteractive(InteractivityMode interactive);
    InteractivityMode GetInteractive() const { return m_interactive; }

    void SetCursor(int position);
    int GetCursor() const { return m_cursor; }

    void SetSelection();  // clear
    void SetSelection(int start, int end);
    bool GetSelection(int *start, int *end) const;

    void SetScroll(const Point &scroll);
    const Point &GetScroll() const { return m_scroll; }

    void SetColorSelection(const Color &color);
    const Color &GetColorSelection() const { return m_color_selection; }

    void SetColorSelected(const Color &color);
    const Color &GetColorSelected() const { return m_color_selected; }

  protected:
    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, GetStaticType()); Frame::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Text(Layout *parent);
    virtual ~Text();
  
    void SizeChanged(EventHandle *handle);
    void UpdateLayout();
    void ScrollToCursor();

    virtual void RenderElement(Renderer *renderer) const;

    std::string m_font;
    std::string m_text;
    float m_size;
    bool m_wordwrap;
    TextLayoutPtr m_layout;

    Color m_color_text;
    Color m_color_selection;
    Color m_color_selected;

    InteractivityMode m_interactive;

    Point m_scroll;

    int m_select;
    int m_cursor;

    // Event handlers for mouse events
    void EventInternal_LeftDown(EventHandle *e);
    void EventInternal_LeftUp(EventHandle *e);
    void EventInternal_Move(EventHandle *e, const Point &pt);

    // Event handlers for key events
    void EventInternal_KeyType(EventHandle *e, const std::string &type);
    void EventInternal_KeyDownOrRepeat(EventHandle *e, const KeyEvent &ev);

    // Lua bindings
    static int luaF_SetText(lua_State *L);
    static int luaF_GetText(lua_State *L);

    static int luaF_SetFont(lua_State *L);
    static int luaF_GetFont(lua_State *L);

    static int luaF_SetSize(lua_State *L);
    static int luaF_GetSize(lua_State *L);

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
