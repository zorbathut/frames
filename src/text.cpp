
#include "frames/text.h"

#include "frames/environment.h"
#include "frames/lua.h"
#include "frames/rect.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include <GL/GLew.h>

namespace Frames {
  Text *Text::CreateBare(Layout *parent) {
    return new Text(parent);
  }
  Text *Text::CreateTagged_imp(const char *filename, int line, Layout *parent) {
    Text *rv = new Text(parent);
    rv->SetNameStatic(filename);
    rv->SetNameId(line);
    return rv;
  }

  /*static*/ const char *Text::GetStaticType() {
    return "Text";
  }

  void Text::SetText(const std::string &text) {
    if (m_text != text) {
      m_text = text;
      
      m_cursor = std::min(m_cursor, (int)m_text.size()); // UNICODE TODO
      m_select = std::min(m_select, (int)m_text.size()); // UNICODE TODO

      UpdateLayout();
    }
  }

  void Text::SetFont(const std::string &id) {
    if (m_font != id) {
      m_font = id;

      UpdateLayout();
    }
  }

  void Text::SetSize(float size) {
    if (m_size != size) {
      m_size = size;

      UpdateLayout();
    }
  }

  void Text::SetWordwrap(bool wordwrap) {
    if (m_wordwrap != wordwrap) {
      m_wordwrap = wordwrap;

      UpdateLayout();
    }
  }

  void Text::SetColor(const Color &color) {
    m_color_text = color;
    // no need to update layout, this hasn't changed the layout at all
  }

  void Text::SetInteractive(InteractivityMode interactive) {
    m_interactive = interactive;
    // clear focus if necessary

    // clear event handlers
    EventMouseLeftDownDetach(Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftDown));
    EventMouseLeftUpDetach(Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftUp));
    EventMouseLeftUpOutsideDetach(Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftUp));

    // if necessary, insert event handlers
    if (interactive == INTERACTIVE_SELECT || interactive == INTERACTIVE_EDIT) {
      EventMouseLeftDownAttach(Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftDown));
      EventMouseLeftUpAttach(Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftUp));
      EventMouseLeftUpOutsideAttach(Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftUp));
    }
  }

  void Text::SetCursor(int position) {
    if (m_cursor == position) {
      // hey sweet we're done
    } else if (m_select == position) {
      std::swap(m_select, m_cursor);
    } else {
      // wipe out the select since it's incompatible with this
      m_select = m_cursor = position;
    }

    m_cursor = Utility::Clamp(m_cursor, 0, m_text.size());

    ScrollToCursor();
  }

  void Text::SetSelection() {
    m_select = m_cursor;
  }

  void Text::SetSelection(int start, int end) {
    // clamp to sane values
    start = Utility::Clamp(start, 0, m_text.size());  // UNICODE TODO
    end = Utility::Clamp(end, 0, m_text.size());  // UNICODE TODO
    if (start == end) {
      m_select = m_cursor = start;
    } else if (m_cursor == start) {
      m_select = end;
    } else if (m_cursor == end) {
      m_select = start;
    } else {
      m_select = std::min(start, end);
      m_cursor = std::max(start, end);
    }
  }

  bool Text::GetSelection(int *start, int *end) const {
    *start = std::min(m_select, m_cursor);
    *end = std::max(m_select, m_cursor);

    return m_select != m_cursor;
  }

  void Text::SetScroll(const Point &scroll) {
    m_scroll = scroll;
  }

  void Text::SetColorSelection(const Color &color) {
    m_color_selection = color;
  }

  void Text::SetColorSelected(const Color &color) {
    m_color_selected = color;
  }

  /*static*/ void Text::l_RegisterFunctions(lua_State *L) {
    Frame::l_RegisterFunctions(L);

    l_RegisterFunction(L, GetStaticType(), "SetText", l_SetText);
    l_RegisterFunction(L, GetStaticType(), "GetText", l_GetText);

    l_RegisterFunction(L, GetStaticType(), "SetFont", l_SetFont);
    l_RegisterFunction(L, GetStaticType(), "GetFont", l_GetFont);

    l_RegisterFunction(L, GetStaticType(), "SetSize", l_SetSize);
    l_RegisterFunction(L, GetStaticType(), "GetSize", l_GetSize);

    l_RegisterFunction(L, GetStaticType(), "SetWordwrap", l_SetWordwrap);
    l_RegisterFunction(L, GetStaticType(), "GetWordwrap", l_GetWordwrap);

    l_RegisterFunction(L, GetStaticType(), "SetColor", l_SetColor);
    l_RegisterFunction(L, GetStaticType(), "GetColor", l_GetColor);

    l_RegisterFunction(L, GetStaticType(), "SetInteractive", l_SetInteractive);
    l_RegisterFunction(L, GetStaticType(), "GetInteractive", l_GetInteractive);

    l_RegisterFunction(L, GetStaticType(), "SetCursor", l_SetCursor);
    l_RegisterFunction(L, GetStaticType(), "GetCursor", l_GetCursor);

    l_RegisterFunction(L, GetStaticType(), "SetSelection", l_SetSelection);
    l_RegisterFunction(L, GetStaticType(), "GetSelection", l_GetSelection);

    l_RegisterFunction(L, GetStaticType(), "SetScroll", l_SetScroll);
    l_RegisterFunction(L, GetStaticType(), "GetScroll", l_GetScroll);

    l_RegisterFunction(L, GetStaticType(), "SetColorSelection", l_SetColorSelection);
    l_RegisterFunction(L, GetStaticType(), "GetColorSelection", l_GetColorSelection);

    l_RegisterFunction(L, GetStaticType(), "SetColorSelected", l_SetColorSelected);
    l_RegisterFunction(L, GetStaticType(), "GetColorSelected", l_GetColorSelected);
  }

  Text::Text(Layout *parent) :
      Frame(parent),
      m_size(16),
      m_wordwrap(false),
      m_color_text(1, 1, 1),
      m_color_selection(0.3, 0.3, 0.5, 0.5),
      m_color_selected(1, 1, 1),
      m_interactive(INTERACTIVE_NONE),
      m_scroll(0, 0),
      m_select(0),
      m_cursor(0)
  {
    m_font = GetEnvironment()->GetConfiguration().fontDefaultId;

    SetWidthDefault(0);
    SetHeightDefault(20);

    // default font
    UpdateLayout();

    // TODO: We really only need to update fullheight, and that only if wordwrap is enabled. Worry about this later.
    // There's a bit of a potential performance issue here - imagine we have a wordwrapped text field of the wrong (too small) size, with the width set and the height unset.
    // We clear the width.
    // This function triggers.
    // The function will change the default width and height. Now the size has changed again! So we trigger this function again!
    // Now, luckily, all the stuff it does is highly cached - each lookup is essentially two bimap lookups and a little muckery with smart pointers. So this isn't a catastrophe. But still it's not really ideal.
    // Maybe we should cache the TextInfoPtr?
    EventSizeAttach(Delegate<void (EventHandle *handle)>(this, &Text::SizeChanged));
  };

  Text::~Text() { };
  
  void Text::SizeChanged(EventHandle *handle) {
    UpdateLayout();
  }

  void Text::UpdateLayout() {
    if (m_font.empty() && m_text.empty()) {
    } else if (m_font.empty()) {
      // PROBLEM
      GetEnvironment()->LogError("Error - attempting to render text without a valid font");
    } else {
      TextInfoPtr tinfo = GetEnvironment()->GetTextManager()->GetTextInfo(m_font, m_size, m_text);
      SetWidthDefault(tinfo->GetFullWidth());

      m_layout = tinfo->GetLayout(GetWidth(), m_wordwrap);
      SetHeightDefault(m_layout->GetFullHeight());

      ScrollToCursor();
    }
  }

  void Text::ScrollToCursor() {
    /*if (!false) {  // TODO: focus
      return;
    }*/

    Point tpos = m_layout->GetCoordinateFromCharacter(m_cursor);
    Point cscroll = GetScroll();

    // First, do the X axis
    if (!m_wordwrap) {
      cscroll.x = Utility::Clamp(Utility::Clamp(cscroll.x, tpos.x - GetWidth() * 3 / 4, tpos.x - GetWidth() / 4), 0, m_layout->GetParent()->GetFullWidth() - GetWidth());
    } else {
      cscroll.x = 0;
    }

    // Next, do the Y axis
    float lineheight = m_layout->GetParent()->GetParent()->GetLineHeight(m_size);
    if (GetHeight() >= lineheight) {
      cscroll.y = Utility::Clamp(cscroll.y, tpos.y + lineheight - GetHeight(), tpos.y);
    } else {
      cscroll.y = tpos.y + (lineheight - GetHeight()) / 2;
    }

    SetScroll(cscroll);
  }

  void Text::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // we'll fix this up further later
    if (m_layout) {
      Rect bounds = GetBounds();

      // render selection
      if (m_interactive >= INTERACTIVE_SELECT && m_cursor != m_select) {
        renderer->SetTexture();

        int s = m_cursor;
        int e = m_select;
        if (s > e) std::swap(s, e);

        int sl = m_layout->GetLineFromCharacter(s);
        int el = m_layout->GetLineFromCharacter(e);

        renderer->SetTexture();
        Renderer::Vertex *verts = renderer->Request((el - sl + 1) * 4);
        int idx = 0;
        for (int i = sl; i <= el; ++i) {
          int ts = std::max(s, i ? (m_layout->GetEOLFromLine(i - 1) + 1) : 0);
          int te = std::min(m_layout->GetEOLFromLine(i), e);

          Rect rect;
          rect.s = m_layout->GetCoordinateFromCharacter(ts);
          rect.e = m_layout->GetCoordinateFromCharacter(te);

          rect.s -= m_scroll;
          rect.e -= m_scroll;

          rect.s.x += GetLeft();
          rect.e.x += GetLeft();

          rect.s.y += GetTop();
          rect.e.y += GetTop();

          rect.e.y += m_layout->GetParent()->GetParent()->GetLineHeight(m_size);

          if (Renderer::WriteCroppedRect(verts + idx, rect, m_color_selection, bounds)) {
            idx += 4;
          }
        }
        renderer->Return(GL_QUADS, idx);
      }

      // render the actual text
      m_layout->Render(renderer, m_color_text, GetBounds(), GetScroll());

      // render cursor, if there is one
      if (m_interactive >= INTERACTIVE_CURSOR && GetFocus()) { // display only if in focus
        // TODO: cull properly when too small
        renderer->SetTexture();
        Renderer::Vertex *vert = renderer->Request(4);
        
        Point origin = m_layout->GetCoordinateFromCharacter(m_cursor) - m_scroll;
        origin.x += GetLeft();
        origin.y += GetTop();
        
        Renderer::WriteCroppedTexRect(vert, Rect(origin, origin + Point(1, m_layout->GetParent()->GetParent()->GetLineHeightFirst(m_size))), Rect(), Color(1, 1, 1), bounds);

        renderer->Return(GL_QUADS);
      }
    }
  }

  void Text::EventInternal_LeftDown(EventHandle *e) {
    int pos = m_layout->GetCharacterFromCoordinate(GetEnvironment()->GetMouse() + m_scroll - Point(GetLeft(), GetTop()));
    SetCursor(pos);
    SetSelection();

    EventMouseMoveAttach(Delegate<void (EventHandle *, const Point &pt)>(this, &Text::EventInternal_Move));
    EventMouseMoveOutsideAttach(Delegate<void (EventHandle *, const Point &pt)>(this, &Text::EventInternal_Move));
  }

  void Text::EventInternal_LeftUp(EventHandle *e) {
    int pos = m_layout->GetCharacterFromCoordinate(GetEnvironment()->GetMouse() + m_scroll - Point(GetLeft(), GetTop()));

    // We want to change the cursor position, but still preserve the selection, which takes a little effort
    int start, end;
    GetSelection(&start, &end);
    if (start == GetCursor()) {
      SetSelection(pos, end);
    } else {
      SetSelection(start, pos);
    }
    SetCursor(pos);

    EventMouseMoveDetach(Delegate<void (EventHandle *, const Point &pt)>(this, &Text::EventInternal_Move));
    EventMouseMoveOutsideDetach(Delegate<void (EventHandle *, const Point &pt)>(this, &Text::EventInternal_Move));
  }

  void Text::EventInternal_Move(EventHandle *e, const Point &pt) {
    int pos = m_layout->GetCharacterFromCoordinate(pt + m_scroll - Point(GetLeft(), GetTop()));

    // We want to change the cursor position, but still preserve the selection, which takes a little effort
    int start, end;
    GetSelection(&start, &end);
    if (start == GetCursor()) {
      SetSelection(pos, end);
    } else {
      SetSelection(start, pos);
    }
    SetCursor(pos);
  }

  /*static*/ int Text::l_SetText(lua_State *L) {
    l_checkparams(L, 2);
    Text *self = l_checkframe<Text>(L, 1);

    self->SetText(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Text::l_GetText(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    lua_pushstring(L, self->GetText().c_str());

    return 1;
  }

  /*static*/ int Text::l_SetFont(lua_State *L) {
    l_checkparams(L, 2);
    Text *self = l_checkframe<Text>(L, 1);

    self->SetFont(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Text::l_GetFont(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    lua_pushstring(L, self->GetFont().c_str());

    return 1;
  }

  /*static*/ int Text::l_SetSize(lua_State *L) {
    l_checkparams(L, 2);
    Text *self = l_checkframe<Text>(L, 1);

    self->SetSize(luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Text::l_GetSize(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->GetSize());

    return 1;
  }

  /*static*/ int Text::l_SetWordwrap(lua_State *L) {
    l_checkparams(L, 2);
    Text *self = l_checkframe<Text>(L, 1);

    luaL_checktype(L, 2, LUA_TBOOLEAN); // sigh
    self->SetWordwrap(lua_toboolean(L, 2));

    return 0;
  }

  /*static*/ int Text::l_GetWordwrap(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    lua_pushboolean(L, self->GetWordwrap());

    return 1;
  }

  /*static*/ int Text::l_SetColor(lua_State *L) {
    l_checkparams(L, 4, 5);
    Text *self = l_checkframe<Text>(L, 1);

    self->SetColor(Color(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::l_GetColor(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    const Color &col = self->GetColor();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }

  /*static*/ int Text::l_SetInteractive(lua_State *L) {
    l_checkparams(L, 2);
    Text *self = l_checkframe<Text>(L, 1);

    const char *param = lua_tostring(L, 2);
    if (strcmp(param, "none") == 0) {
      self->SetInteractive(INTERACTIVE_NONE);
    } else if (strcmp(param, "select") == 0) {
      self->SetInteractive(INTERACTIVE_SELECT);
    } else if (strcmp(param, "edit") == 0) {
      self->SetInteractive(INTERACTIVE_EDIT);
    } else {
      luaL_error(L, "Invalid interaction type");
    }

    return 0;
  }

  /*static*/ int Text::l_GetInteractive(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    if (self->GetInteractive() == INTERACTIVE_NONE) {
      lua_pushliteral(L, "none");
    } else if (self->GetInteractive() == INTERACTIVE_SELECT) {
      lua_pushliteral(L, "select");
    } else if (self->GetInteractive() == INTERACTIVE_EDIT) {
      lua_pushliteral(L, "edit");
    } else {
      self->GetEnvironment()->LogError("Completely unknown interactivity flag, please report as a bug");
      lua_pushnil(L);
    }

    return 1;
  }

  /*static*/ int Text::l_SetCursor(lua_State *L) {
    l_checkparams(L, 2);
    Text *self = l_checkframe<Text>(L, 1);

    self->SetCursor(luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Text::l_GetCursor(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->GetCursor());

    return 1;
  }

  /*static*/ int Text::l_SetSelection(lua_State *L) {
    l_checkparams(L, 1, 3);
    Text *self = l_checkframe<Text>(L, 1);

    if (lua_gettop(L) == 1) {
      self->SetSelection();
    } else if(lua_gettop(L) == 3) {
      self->SetSelection(luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    } else {
      luaL_error(L, "Incorrect number of parameters, expecting 1 or 3");
    }

    return 0;
  }

  /*static*/ int Text::l_GetSelection(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    int s, e;
    if (self->GetSelection(&s, &e)) {
      lua_pushnumber(L, s);
      lua_pushnumber(L, e);
    } else {
      lua_pushnil(L);
      lua_pushnil(L);
    }

    return 2;
  }

  /*static*/ int Text::l_SetScroll(lua_State *L) {
    l_checkparams(L, 3);
    Text *self = l_checkframe<Text>(L, 1);

    self->SetScroll(Point(luaL_checknumber(L, 2), luaL_checknumber(L, 3)));

    return 0;
  }

  /*static*/ int Text::l_GetScroll(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->GetScroll().x);
    lua_pushnumber(L, self->GetScroll().y);

    return 2;
  }

  /*static*/ int Text::l_SetColorSelection(lua_State *L) {
    l_checkparams(L, 4, 5);
    Text *self = l_checkframe<Text>(L, 1);

    self->SetColorSelection(Color(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::l_GetColorSelection(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    const Color &col = self->GetColorSelection();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }

  /*static*/ int Text::l_SetColorSelected(lua_State *L) {
    l_checkparams(L, 4, 5);
    Text *self = l_checkframe<Text>(L, 1);

    self->SetColorSelected(Color(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::l_GetColorSelected(lua_State *L) {
    l_checkparams(L, 1);
    Text *self = l_checkframe<Text>(L, 1);

    const Color &col = self->GetColorSelected();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }
}

