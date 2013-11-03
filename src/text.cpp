
#include "frames/text.h"

#include "frames/environment.h"
#include "frames/lua.h"
#include "frames/rect.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include "frames/os_gl.h"

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
    
    // kill focus if we no longer need to be focused
    if (interactive == INTERACTIVE_NONE) {
      if (GetEnvironment()->GetFocus() == this) {
        GetEnvironment()->SetFocus(0);
      }
    }

    // clear event handlers
    EventDetach(Event::MouseLeftDown, Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftDown));
    EventDetach(Event::MouseLeftUp, Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftUp));
    EventDetach(Event::MouseLeftUpoutside, Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftUp));

    EventDetach(Event::KeyDown, Delegate<void (EventHandle *, const KeyEvent &)>(this, &Text::EventInternal_KeyDownOrRepeat));
    EventDetach(Event::KeyRepeat, Delegate<void (EventHandle *, const KeyEvent &)>(this, &Text::EventInternal_KeyDownOrRepeat));
    EventDetach(Event::KeyType, Delegate<void (EventHandle *, const std::string &)>(this, &Text::EventInternal_KeyType));

    // if necessary, insert event handlers
    if (interactive == INTERACTIVE_SELECT || interactive == INTERACTIVE_CURSOR || interactive == INTERACTIVE_EDIT) {
      EventAttach(Event::MouseLeftDown, Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftDown));
      EventAttach(Event::MouseLeftUp, Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftUp));
      EventAttach(Event::MouseLeftUpoutside, Delegate<void (EventHandle *)>(this, &Text::EventInternal_LeftUp));
    
      // These are needed mostly for ctrl-C
      EventAttach(Event::KeyDown, Delegate<void (EventHandle *, const KeyEvent &)>(this, &Text::EventInternal_KeyDownOrRepeat));
      EventAttach(Event::KeyRepeat, Delegate<void (EventHandle *, const KeyEvent &)>(this, &Text::EventInternal_KeyDownOrRepeat));
      EventAttach(Event::KeyType, Delegate<void (EventHandle *, const std::string &)>(this, &Text::EventInternal_KeyType));
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

  /*static*/ void Text::luaF_RegisterFunctions(lua_State *L) {
    Frame::luaF_RegisterFunctions(L);

    luaF_RegisterFunction(L, GetStaticType(), "SetText", luaF_SetText);
    luaF_RegisterFunction(L, GetStaticType(), "GetText", luaF_GetText);

    luaF_RegisterFunction(L, GetStaticType(), "SetFont", luaF_SetFont);
    luaF_RegisterFunction(L, GetStaticType(), "GetFont", luaF_GetFont);

    luaF_RegisterFunction(L, GetStaticType(), "SetSize", luaF_SetSize);
    luaF_RegisterFunction(L, GetStaticType(), "GetSize", luaF_GetSize);

    luaF_RegisterFunction(L, GetStaticType(), "SetWordwrap", luaF_SetWordwrap);
    luaF_RegisterFunction(L, GetStaticType(), "GetWordwrap", luaF_GetWordwrap);

    luaF_RegisterFunction(L, GetStaticType(), "SetColor", luaF_SetColor);
    luaF_RegisterFunction(L, GetStaticType(), "GetColor", luaF_GetColor);

    luaF_RegisterFunction(L, GetStaticType(), "SetInteractive", luaF_SetInteractive);
    luaF_RegisterFunction(L, GetStaticType(), "GetInteractive", luaF_GetInteractive);

    luaF_RegisterFunction(L, GetStaticType(), "SetCursor", luaF_SetCursor);
    luaF_RegisterFunction(L, GetStaticType(), "GetCursor", luaF_GetCursor);

    luaF_RegisterFunction(L, GetStaticType(), "SetSelection", luaF_SetSelection);
    luaF_RegisterFunction(L, GetStaticType(), "GetSelection", luaF_GetSelection);

    luaF_RegisterFunction(L, GetStaticType(), "SetScroll", luaF_SetScroll);
    luaF_RegisterFunction(L, GetStaticType(), "GetScroll", luaF_GetScroll);

    luaF_RegisterFunction(L, GetStaticType(), "SetColorSelection", luaF_SetColorSelection);
    luaF_RegisterFunction(L, GetStaticType(), "GetColorSelection", luaF_GetColorSelection);

    luaF_RegisterFunction(L, GetStaticType(), "SetColorSelected", luaF_SetColorSelected);
    luaF_RegisterFunction(L, GetStaticType(), "GetColorSelected", luaF_GetColorSelected);
  }

  Text::Text(Layout *parent) :
      Frame(parent),
      m_size(16),
      m_wordwrap(false),
      m_color_text(1, 1, 1),
      m_color_selection(0.3f, 0.3f, 0.5f, 0.5f),
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
    EventAttach(Event::Size, Delegate<void (EventHandle *handle)>(this, &Text::SizeChanged));
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
      cscroll.x = Utility::Clamp(Utility::Clamp(cscroll.x, tpos.x - GetWidth() * 3 / 4, tpos.x - GetWidth() / 4), 0.f, m_layout->GetParent()->GetFullWidth() - GetWidth());
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

          if (Renderer::WriteCroppedRect(verts + idx, rect, m_color_selection * Color(1, 1, 1, renderer->AlphaGet()), bounds)) {
            idx += 4;
          }
        }
        renderer->Return(GL_QUADS, idx);
      }

      // render the actual text
      m_layout->Render(renderer, m_color_text * Color(1, 1, 1, renderer->AlphaGet()), GetBounds(), GetScroll());

      // render cursor, if there is one
      if (m_interactive >= INTERACTIVE_CURSOR && GetFocus()) { // display only if in focus
        // TODO: cull properly when too small
        renderer->SetTexture();
        Renderer::Vertex *vert = renderer->Request(4);
        
        Point origin = m_layout->GetCoordinateFromCharacter(m_cursor) - m_scroll;
        origin.x += GetLeft();
        origin.y += GetTop();
        
        Renderer::WriteCroppedTexRect(vert, Rect(origin, origin + Point(1, m_layout->GetParent()->GetParent()->GetLineHeightFirst(m_size))), Rect(), Color(1, 1, 1, renderer->AlphaGet()), bounds);

        renderer->Return(GL_QUADS);
      }
    }
  }

  void Text::EventInternal_LeftDown(EventHandle *e) {
    int pos = m_layout->GetCharacterFromCoordinate(GetEnvironment()->GetMouse() + m_scroll - Point(GetLeft(), GetTop()));
    SetCursor(pos);
    SetSelection();

    EventAttach(Event::MouseMove, Delegate<void (EventHandle *, const Point &pt)>(this, &Text::EventInternal_Move));
    EventAttach(Event::MouseMoveoutside, Delegate<void (EventHandle *, const Point &pt)>(this, &Text::EventInternal_Move));

    if (m_interactive >= INTERACTIVE_SELECT) {
      GetEnvironment()->SetFocus(this);
    }
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

    EventDetach(Event::MouseMove, Delegate<void (EventHandle *, const Point &pt)>(this, &Text::EventInternal_Move));
    EventDetach(Event::MouseMoveoutside, Delegate<void (EventHandle *, const Point &pt)>(this, &Text::EventInternal_Move));
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

  void Text::EventInternal_KeyType(EventHandle *e, const std::string &type) {
    // First, see if we even can have things enter
    if (m_interactive != INTERACTIVE_EDIT) {
      return;
    }

    if (GetEnvironment()->IsCtrl() || GetEnvironment()->IsAlt()) {
      return;
    }

    int ncursor = std::min(m_cursor, m_select) + type.size();

    SetText(m_text.substr(0, std::min(m_cursor, m_select)) + type + m_text.substr(std::max(m_cursor, m_select)));
    SetCursor(ncursor);
    SetSelection();
  }
  void Text::EventInternal_KeyDownOrRepeat(EventHandle *e, const KeyEvent &ev) {
    // Things supported for everything interactive
    if (ev.key == Key::A && ev.ctrl) {
      SetSelection(0, m_text.size());
      SetCursor(m_text.size());
    } else if ((ev.key == Key::C && ev.ctrl) || (m_interactive == INTERACTIVE_SELECT && ev.key == Key::X && ev.ctrl)) { // if we're in select mode, interpret cut as copy
      // copy to clipboard
      if (m_cursor != m_select) {
        GetEnvironment()->GetConfiguration().clipboard->Set(m_text.substr(std::min(m_cursor, m_select), std::abs(m_cursor - m_select)));
      }
    }

    // todo: pageup
    // todo: pagedown

    if (m_interactive < INTERACTIVE_CURSOR) {
      return;
    }

    {
      // movement-based things
      int newcursor = -1;
      bool hascursor = true;
      if (ev.key == Key::Left) {
        if (ev.ctrl) {
          // shift a word
          int curs = m_cursor - 2;
          for (; curs > 0; curs--) {
            if (isspace(m_text[curs])) {
              curs++; // put us after the space
              break;
            }
          }
          newcursor = curs;
        } else {
          newcursor = m_cursor - 1;
        }
      } else if (ev.key == Key::Right) {
        if (ev.ctrl) {
          // shift a word
          int curs = m_cursor + 1;
          for (; curs < (int)m_text.size(); curs++) {
            if (isspace(m_text[curs])) {
              break;
            }
          }
          newcursor = curs;
        } else {
          newcursor = m_cursor + 1;
        }
      } else if (ev.key == Key::Up) {
        float lineheight = m_layout->GetParent()->GetParent()->GetLineHeight(m_size);
        Point ccor = m_layout->GetCoordinateFromCharacter(m_cursor);
        ccor.y -= lineheight / 2;
        newcursor = m_layout->GetCharacterFromCoordinate(ccor);
      } else if (ev.key == Key::Down) {
        float lineheight = m_layout->GetParent()->GetParent()->GetLineHeight(m_size);
        Point ccor = m_layout->GetCoordinateFromCharacter(m_cursor);
        ccor.y += lineheight * 3 / 2; // need to bump it into the bottom line
        newcursor = m_layout->GetCharacterFromCoordinate(ccor);
      } else if (ev.key == Key::Home) {
        newcursor = 0;
      } else if (ev.key == Key::End) {
        newcursor = m_text.size();
      } else {
        // isn't actually a cursor movement key
        hascursor = false;
      }

      if (hascursor) {
        newcursor = Utility::Clamp(newcursor, 0, m_text.size());
        if (ev.shift) {
          SetSelection(m_select, newcursor);
          SetCursor(newcursor);
        } else {
          SetCursor(newcursor);
          SetSelection();
        }
      }
    }

    if (m_interactive < INTERACTIVE_EDIT) {
      return;
    }

    if (ev.key == Key::Backspace) {
      if (m_cursor != m_select) {
        // wipe out the selection
        int ncursor = std::min(m_cursor, m_select);
        SetText(m_text.substr(0, std::min(m_cursor, m_select)) + m_text.substr(std::max(m_cursor, m_select)));
        SetCursor(ncursor);
        SetSelection();
      } else if (m_cursor) {
        // wipe out the character before the cursor
        int ncursor = m_cursor - 1;
        SetText(m_text.substr(0, m_cursor - 1) + m_text.substr(m_cursor));
        SetCursor(ncursor);
      }
    } else if (ev.key == Key::Delete) {
      if (m_cursor != m_select) {
        // wipe out the selection
        int ncursor = std::min(m_cursor, m_select);
        SetText(m_text.substr(0, std::min(m_cursor, m_select)) + m_text.substr(std::max(m_cursor, m_select)));
        SetCursor(ncursor);
        SetSelection();
      } else if (m_cursor != (int)m_text.size()) {
        // wipe out the character after the cursor
        SetText(m_text.substr(0, m_cursor) + m_text.substr(m_cursor + 1));
      }
    } else if (ev.key == Key::X && ev.ctrl) {
      // cut to clipboard
      if (m_cursor != m_select) {
        GetEnvironment()->GetConfiguration().clipboard->Set(m_text.substr(std::min(m_cursor, m_select), std::abs(m_cursor - m_select)));

        // chop out that text
        int ncursor = std::min(m_cursor, m_select);
        SetText(m_text.substr(0, std::min(m_cursor, m_select)) + m_text.substr(std::max(m_cursor, m_select)));
        SetCursor(ncursor);
        SetSelection();
      }
    } else if (ev.key == Key::V && ev.ctrl) {
      // paste from clipboard
      std::string clipboard = GetEnvironment()->GetConfiguration().clipboard->Get();
      int ncursor = m_cursor + clipboard.size();
      SetText(m_text.substr(0, std::min(m_cursor, m_select)) + clipboard + m_text.substr(std::max(m_cursor, m_select)));
      SetCursor(ncursor);
      SetSelection();
    }
  }

  /*static*/ int Text::luaF_SetText(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SetText(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Text::luaF_GetText(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushstring(L, self->GetText().c_str());

    return 1;
  }

  /*static*/ int Text::luaF_SetFont(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SetFont(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Text::luaF_GetFont(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushstring(L, self->GetFont().c_str());

    return 1;
  }

  /*static*/ int Text::luaF_SetSize(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SetSize((float)luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Text::luaF_GetSize(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->GetSize());

    return 1;
  }

  /*static*/ int Text::luaF_SetWordwrap(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    luaL_checktype(L, 2, LUA_TBOOLEAN); // sigh
    self->SetWordwrap(lua_toboolean(L, 2) != 0);

    return 0;
  }

  /*static*/ int Text::luaF_GetWordwrap(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushboolean(L, self->GetWordwrap());

    return 1;
  }

  /*static*/ int Text::luaF_SetColor(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SetColor(Color((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4), (float)luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::luaF_GetColor(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    const Color &col = self->GetColor();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }

  /*static*/ int Text::luaF_SetInteractive(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

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

  /*static*/ int Text::luaF_GetInteractive(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

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

  /*static*/ int Text::luaF_SetCursor(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SetCursor(luaL_checkinteger(L, 2));

    return 0;
  }

  /*static*/ int Text::luaF_GetCursor(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->GetCursor());

    return 1;
  }

  /*static*/ int Text::luaF_SetSelection(lua_State *L) {
    luaF_checkparams(L, 1, 3);
    Text *self = luaF_checkframe<Text>(L, 1);

    if (lua_gettop(L) == 1) {
      self->SetSelection();
    } else if(lua_gettop(L) == 3) {
      self->SetSelection(luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
    } else {
      luaL_error(L, "Incorrect number of parameters, expecting 1 or 3");
    }

    return 0;
  }

  /*static*/ int Text::luaF_GetSelection(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

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

  /*static*/ int Text::luaF_SetScroll(lua_State *L) {
    luaF_checkparams(L, 3);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SetScroll(Point((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3)));

    return 0;
  }

  /*static*/ int Text::luaF_GetScroll(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->GetScroll().x);
    lua_pushnumber(L, self->GetScroll().y);

    return 2;
  }

  /*static*/ int Text::luaF_SetColorSelection(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SetColorSelection(Color((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4), (float)luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::luaF_GetColorSelection(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    const Color &col = self->GetColorSelection();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }

  /*static*/ int Text::luaF_SetColorSelected(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SetColorSelected(Color((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4), (float)luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::luaF_GetColorSelected(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    const Color &col = self->GetColorSelected();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }
}

