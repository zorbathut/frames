
#include "frames/text.h"

#include "frames/cast.h"
#include "frames/environment.h"
#include "frames/lua.h"
#include "frames/rect.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include "frames/os_gl.h"

namespace Frames {
  FRAMES_DEFINE_RTTI(Text, Frame);

  Text *Text::Create(const std::string &name, Layout *parent) {
    return new Text(name, parent);
  }

  void Text::TextSet(const std::string &text) {
    if (m_text != text) {
      m_text = text;
      
      m_cursor = std::min(m_cursor, (int)m_text.size()); // UNICODE TODO
      m_select = std::min(m_select, (int)m_text.size()); // UNICODE TODO

      UpdateLayout();
    }
  }

  void Text::FontSet(const std::string &id) {
    if (m_font != id) {
      m_font = id;

      UpdateLayout();
    }
  }

  void Text::SizeSet(float size) {
    if (m_size != size) {
      m_size = size;

      UpdateLayout();
    }
  }

  void Text::WordwrapSet(bool wordwrap) {
    if (m_wordwrap != wordwrap) {
      m_wordwrap = wordwrap;

      UpdateLayout();
    }
  }

  void Text::ColorSet(const Color &color) {
    m_color_text = color;
    // no need to update layout, this hasn't changed the layout at all
  }

  void Text::InteractiveSet(InteractivityMode interactive) {
    m_interactive = interactive;
    
    // kill focus if we no longer need to be focused
    if (interactive == INTERACTIVE_NONE) {
      if (EnvironmentGet()->FocusGet() == this) {
        EnvironmentGet()->FocusSet(0);
      }
    }

    // clear event handlers
    EventDetach(Event::MouseLeftDown, Delegate<void (Handle *)>(this, &Text::EventInternal_LeftDown));
    EventDetach(Event::MouseLeftUp, Delegate<void (Handle *)>(this, &Text::EventInternal_LeftUp));
    EventDetach(Event::MouseLeftUpoutside, Delegate<void (Handle *)>(this, &Text::EventInternal_LeftUp));

    EventDetach(Event::KeyDown, Delegate<void (Handle *, Input::Key)>(this, &Text::EventInternal_KeyDownOrRepeat));
    EventDetach(Event::KeyRepeat, Delegate<void (Handle *, Input::Key)>(this, &Text::EventInternal_KeyDownOrRepeat));
    EventDetach(Event::KeyText, Delegate<void (Handle *, const std::string &)>(this, &Text::EventInternal_KeyText));

    // if necessary, insert event handlers
    if (interactive == INTERACTIVE_SELECT || interactive == INTERACTIVE_CURSOR || interactive == INTERACTIVE_EDIT) {
      EventAttach(Event::MouseLeftDown, Delegate<void (Handle *)>(this, &Text::EventInternal_LeftDown));
      EventAttach(Event::MouseLeftUp, Delegate<void (Handle *)>(this, &Text::EventInternal_LeftUp));
      EventAttach(Event::MouseLeftUpoutside, Delegate<void (Handle *)>(this, &Text::EventInternal_LeftUp));
    
      // These are needed mostly for ctrl-C
      EventAttach(Event::KeyDown, Delegate<void (Handle *, Input::Key)>(this, &Text::EventInternal_KeyDownOrRepeat));
      EventAttach(Event::KeyRepeat, Delegate<void (Handle *, Input::Key)>(this, &Text::EventInternal_KeyDownOrRepeat));
      EventAttach(Event::KeyText, Delegate<void (Handle *, const std::string &)>(this, &Text::EventInternal_KeyText));
    }
  }

  void Text::CursorSet(int position) {
    if (m_cursor == position) {
      // hey sweet we're done
    } else if (m_select == position) {
      std::swap(m_select, m_cursor);
    } else {
      // wipe out the select since it's incompatible with this
      m_select = m_cursor = position;
    }

    m_cursor = detail::Clamp(m_cursor, 0, (int)m_text.size());

    ScrollToCursor();
  }

  void Text::SelectionClear() {
    m_select = m_cursor;
  }

  void Text::SelectionSet(int start, int end) {
    // clamp to sane values
    start = detail::Clamp(start, 0, (int)m_text.size());  // UNICODE TODO
    end = detail::Clamp(end, 0, (int)m_text.size());  // UNICODE TODO
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

  bool Text::SelectionGet(int *start, int *end) const {
    *start = std::min(m_select, m_cursor);
    *end = std::max(m_select, m_cursor);

    return m_select != m_cursor;
  }

  void Text::ScrollSet(const Vector &scroll) {
    m_scroll = scroll;
  }

  void Text::ColorSelectionSet(const Color &color) {
    m_color_selection = color;
  }

  void Text::ColorSelectedSet(const Color &color) {
    m_color_selected = color;
  }

  /*static*/ void Text::luaF_RegisterFunctions(lua_State *L) {
    Frame::luaF_RegisterFunctions(L);

    luaF_RegisterFunction(L, TypeStaticGet(), "TextSet", luaF_SetText);
    luaF_RegisterFunction(L, TypeStaticGet(), "TextGet", luaF_GetText);

    luaF_RegisterFunction(L, TypeStaticGet(), "FontSet", luaF_SetFont);
    luaF_RegisterFunction(L, TypeStaticGet(), "GetFont", luaF_GetFont);

    luaF_RegisterFunction(L, TypeStaticGet(), "SizeSet", luaF_SizeSet);
    luaF_RegisterFunction(L, TypeStaticGet(), "SizeGet", luaF_SizeGet);

    luaF_RegisterFunction(L, TypeStaticGet(), "WordwrapSet", luaF_SetWordwrap);
    luaF_RegisterFunction(L, TypeStaticGet(), "WordwrapGet", luaF_GetWordwrap);

    luaF_RegisterFunction(L, TypeStaticGet(), "ColorSet", luaF_SetColor);
    luaF_RegisterFunction(L, TypeStaticGet(), "ColorGet", luaF_GetColor);

    luaF_RegisterFunction(L, TypeStaticGet(), "InteractiveSet", luaF_SetInteractive);
    luaF_RegisterFunction(L, TypeStaticGet(), "InteractiveGet", luaF_GetInteractive);

    luaF_RegisterFunction(L, TypeStaticGet(), "CursorSet", luaF_SetCursor);
    luaF_RegisterFunction(L, TypeStaticGet(), "CursorGet", luaF_GetCursor);

    luaF_RegisterFunction(L, TypeStaticGet(), "SelectionSet", luaF_SetSelection);
    luaF_RegisterFunction(L, TypeStaticGet(), "SelectionGet", luaF_GetSelection);

    luaF_RegisterFunction(L, TypeStaticGet(), "ScrollSet", luaF_SetScroll);
    luaF_RegisterFunction(L, TypeStaticGet(), "ScrollGet", luaF_GetScroll);

    luaF_RegisterFunction(L, TypeStaticGet(), "ColorSelectionSet", luaF_SetColorSelection);
    luaF_RegisterFunction(L, TypeStaticGet(), "ColorSelectionGet", luaF_GetColorSelection);

    luaF_RegisterFunction(L, TypeStaticGet(), "ColorSelectedSet", luaF_SetColorSelected);
    luaF_RegisterFunction(L, TypeStaticGet(), "ColorSelectedGet", luaF_GetColorSelected);
  }

  Text::Text(const std::string &name, Layout *parent) :
      Frame(name, parent),
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
    m_font = EnvironmentGet()->ConfigurationGet().fontDefaultId;

    WidthDefaultSet(0);
    HeightDefaultSet(20);

    // default font
    UpdateLayout();

    // TODO: We really only need to update fullheight, and that only if wordwrap is enabled. Worry about this later.
    // There's a bit of a potential performance issue here - imagine we have a wordwrapped text field of the wrong (too small) size, with the width set and the height unset.
    // We clear the width.
    // This function triggers.
    // The function will change the default width and height. Now the size has changed again! So we trigger this function again!
    // Now, luckily, all the stuff it does is highly cached - each lookup is essentially two bimap lookups and a little muckery with smart pointers. So this isn't a catastrophe. But still it's not really ideal.
    // Maybe we should cache the TextInfoPtr?
    EventAttach(Event::Size, Delegate<void (Handle *handle)>(this, &Text::SizeChanged));
  };

  Text::~Text() { };
  
  void Text::SizeChanged(Handle *handle) {
    UpdateLayout();
  }

  void Text::UpdateLayout() {
    if (m_font.empty() && m_text.empty()) {
    } else if (m_font.empty()) {
      // PROBLEM
      EnvironmentGet()->LogError("Error - attempting to render text without a valid font");
    } else {
      detail::TextInfoPtr tinfo = EnvironmentGet()->GetTextManager()->GetTextInfo(m_font, m_size, m_text);
      WidthDefaultSet(tinfo->GetFullWidth());

      m_layout = tinfo->GetLayout(WidthGet(), m_wordwrap);
      HeightDefaultSet(m_layout->GetFullHeight());

      ScrollToCursor();
    }
  }

  void Text::ScrollToCursor() {
    /*if (!false) {  // TODO: focus
      return;
    }*/

    Vector tpos = m_layout->GetCoordinateFromCharacter(m_cursor);
    Vector cscroll = ScrollGet();

    // First, do the X axis
    if (!m_wordwrap) {
      cscroll.x = detail::Clamp(detail::Clamp(cscroll.x, tpos.x - WidthGet() * 3 / 4, tpos.x - WidthGet() / 4), 0.f, m_layout->ParentGet()->GetFullWidth() - WidthGet());
    } else {
      cscroll.x = 0;
    }

    // Next, do the Y axis
    float lineheight = m_layout->ParentGet()->ParentGet()->GetLineHeight(m_size);
    if (HeightGet() >= lineheight) {
      cscroll.y = detail::Clamp(cscroll.y, tpos.y + lineheight - HeightGet(), tpos.y);
    } else {
      cscroll.y = tpos.y + (lineheight - HeightGet()) / 2;
    }

    ScrollSet(cscroll);
  }

  void Text::RenderElement(detail::Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // we'll fix this up further later
    if (m_layout) {
      Rect bounds = BoundsGet();

      // render selection
      if (m_interactive >= INTERACTIVE_SELECT && m_cursor != m_select) {
        renderer->TextureSet();

        int s = m_cursor;
        int e = m_select;
        if (s > e) std::swap(s, e);

        int sl = m_layout->GetLineFromCharacter(s);
        int el = m_layout->GetLineFromCharacter(e);

        renderer->TextureSet();
        detail::Renderer::Vertex *verts = renderer->Request((el - sl + 1) * 4);
        int idx = 0;
        for (int i = sl; i <= el; ++i) {
          int ts = std::max(s, i ? (m_layout->GetEOLFromLine(i - 1) + 1) : 0);
          int te = std::min(m_layout->GetEOLFromLine(i), e);

          Rect rect;
          rect.s = m_layout->GetCoordinateFromCharacter(ts);
          rect.e = m_layout->GetCoordinateFromCharacter(te);

          rect.s -= m_scroll;
          rect.e -= m_scroll;

          rect.s.x += LeftGet();
          rect.e.x += LeftGet();

          rect.s.y += TopGet();
          rect.e.y += TopGet();

          rect.e.y += m_layout->ParentGet()->ParentGet()->GetLineHeight(m_size);

          if (detail::Renderer::WriteCroppedRect(verts + idx, rect, m_color_selection * Color(1, 1, 1, renderer->AlphaGet()), bounds)) {
            idx += 4;
          }
        }
        renderer->Return(GL_QUADS, idx);
      }

      // render the actual text
      m_layout->Render(renderer, m_color_text * Color(1, 1, 1, renderer->AlphaGet()), BoundsGet(), ScrollGet());

      // render cursor, if there is one
      if (m_interactive >= INTERACTIVE_CURSOR && EnvironmentGet()->FocusGet() == this) { // display only if in focus
        // TODO: cull properly when too small
        renderer->TextureSet();
        detail::Renderer::Vertex *vert = renderer->Request(4);
        
        Vector origin = m_layout->GetCoordinateFromCharacter(m_cursor) - m_scroll;
        origin.x += LeftGet();
        origin.y += TopGet();
        
        detail::Renderer::WriteCroppedTexRect(vert, Rect(origin, origin + Vector(1, m_layout->ParentGet()->ParentGet()->GetLineHeightFirst(m_size))), Rect(), Color(1, 1, 1, renderer->AlphaGet()), bounds);

        renderer->Return(GL_QUADS);
      }
    }
  }

  void Text::EventInternal_LeftDown(Handle *e) {
    int pos = m_layout->GetCharacterFromCoordinate(EnvironmentGet()->Input_MouseGet() + m_scroll - Vector(LeftGet(), TopGet()));
    CursorSet(pos);
    SelectionClear();

    EventAttach(Event::MouseMove, Delegate<void (Handle *, const Vector &pt)>(this, &Text::EventInternal_Move));
    EventAttach(Event::MouseMoveoutside, Delegate<void (Handle *, const Vector &pt)>(this, &Text::EventInternal_Move));

    if (m_interactive >= INTERACTIVE_SELECT) {
      EnvironmentGet()->FocusSet(this);
    }
  }

  void Text::EventInternal_LeftUp(Handle *e) {
    int pos = m_layout->GetCharacterFromCoordinate(EnvironmentGet()->Input_MouseGet() + m_scroll - Vector(LeftGet(), TopGet()));

    // We want to change the cursor position, but still preserve the selection, which takes a little effort
    int start, end;
    SelectionGet(&start, &end);
    if (start == CursorGet()) {
      SelectionSet(pos, end);
    } else {
      SelectionSet(start, pos);
    }
    CursorSet(pos);

    EventDetach(Event::MouseMove, Delegate<void (Handle *, const Vector &pt)>(this, &Text::EventInternal_Move));
    EventDetach(Event::MouseMoveoutside, Delegate<void (Handle *, const Vector &pt)>(this, &Text::EventInternal_Move));
  }

  void Text::EventInternal_Move(Handle *e, const Vector &pt) {
    int pos = m_layout->GetCharacterFromCoordinate(pt + m_scroll - Vector(LeftGet(), TopGet()));

    // We want to change the cursor position, but still preserve the selection, which takes a little effort
    int start, end;
    SelectionGet(&start, &end);
    if (start == CursorGet()) {
      SelectionSet(pos, end);
    } else {
      SelectionSet(start, pos);
    }
    CursorSet(pos);
  }

  void Text::EventInternal_KeyText(Handle *e, const std::string &type) {
    // First, see if we even can have things enter
    if (m_interactive != INTERACTIVE_EDIT) {
      return;
    }

    // If the user is holding ctrl or alt, interpret this as a special command and don't add it to the buffer.
    if (EnvironmentGet()->Input_MetaGet().ctrl || EnvironmentGet()->Input_MetaGet().alt) {
      return;
    }

    int ncursor = std::min(m_cursor, m_select) + (int)type.size();

    TextSet(m_text.substr(0, std::min(m_cursor, m_select)) + type + m_text.substr(std::max(m_cursor, m_select)));
    CursorSet(ncursor);
    SelectionClear();
  }
  void Text::EventInternal_KeyDownOrRepeat(Handle *e, Input::Key key) {
    // Things supported for everything interactive
    if (key == Input::Key::A && EnvironmentGet()->Input_MetaGet().ctrl) {
      SelectionSet(0, (int)m_text.size());
      CursorSet((int)m_text.size());
    } else if ((key == Input::Key::C && EnvironmentGet()->Input_MetaGet().ctrl) || (m_interactive == INTERACTIVE_SELECT && key == Input::Key::X && EnvironmentGet()->Input_MetaGet().ctrl)) { // if we're in select mode, interpret cut as copy
      // copy to clipboard
      if (m_cursor != m_select) {
        EnvironmentGet()->ConfigurationGet().clipboard->Set(m_text.substr(std::min(m_cursor, m_select), std::abs(m_cursor - m_select)));
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
      if (key == Input::Key::Left) {
        if (EnvironmentGet()->Input_MetaGet().ctrl) {
          // shift a word
          int curs = m_cursor - 2;
          for (; curs > 0; --curs) {
            if (isspace(m_text[curs])) {
              curs++; // put us after the space
              break;
            }
          }
          newcursor = curs;
        } else {
          newcursor = m_cursor - 1;
        }
      } else if (key == Input::Key::Right) {
        if (EnvironmentGet()->Input_MetaGet().ctrl) {
          // shift a word
          int curs = m_cursor + 1;
          for (; curs < (int)m_text.size(); ++curs) {
            if (isspace(m_text[curs])) {
              break;
            }
          }
          newcursor = curs;
        } else {
          newcursor = m_cursor + 1;
        }
      } else if (key == Input::Key::Up) {
        float lineheight = m_layout->ParentGet()->ParentGet()->GetLineHeight(m_size);
        Vector ccor = m_layout->GetCoordinateFromCharacter(m_cursor);
        ccor.y -= lineheight / 2;
        newcursor = m_layout->GetCharacterFromCoordinate(ccor);
      } else if (key == Input::Key::Down) {
        float lineheight = m_layout->ParentGet()->ParentGet()->GetLineHeight(m_size);
        Vector ccor = m_layout->GetCoordinateFromCharacter(m_cursor);
        ccor.y += lineheight * 3 / 2; // need to bump it into the bottom line
        newcursor = m_layout->GetCharacterFromCoordinate(ccor);
      } else if (key == Input::Key::Home) {
        newcursor = 0;
      } else if (key == Input::Key::End) {
        newcursor = (int)m_text.size();
      } else {
        // isn't actually a cursor movement key
        hascursor = false;
      }

      if (hascursor) {
        newcursor = detail::Clamp(newcursor, 0, (int)m_text.size());
        if (EnvironmentGet()->Input_MetaGet().shift) {
          SelectionSet(m_select, newcursor);
          CursorSet(newcursor);
        } else {
          CursorSet(newcursor);
          SelectionClear();
        }
      }
    }

    if (m_interactive < INTERACTIVE_EDIT) {
      return;
    }

    if (key == Input::Key::Backspace) {
      if (m_cursor != m_select) {
        // wipe out the selection
        int ncursor = std::min(m_cursor, m_select);
        TextSet(m_text.substr(0, std::min(m_cursor, m_select)) + m_text.substr(std::max(m_cursor, m_select)));
        CursorSet(ncursor);
        SelectionClear();
      } else if (m_cursor) {
        // wipe out the character before the cursor
        int ncursor = m_cursor - 1;
        TextSet(m_text.substr(0, m_cursor - 1) + m_text.substr(m_cursor));
        CursorSet(ncursor);
      }
    } else if (key == Input::Key::Delete) {
      if (m_cursor != m_select) {
        // wipe out the selection
        int ncursor = std::min(m_cursor, m_select);
        TextSet(m_text.substr(0, std::min(m_cursor, m_select)) + m_text.substr(std::max(m_cursor, m_select)));
        CursorSet(ncursor);
        SelectionClear();
      } else if (m_cursor != (int)m_text.size()) {
        // wipe out the character after the cursor
        TextSet(m_text.substr(0, m_cursor) + m_text.substr(m_cursor + 1));
      }
    } else if (key == Input::Key::X && EnvironmentGet()->Input_MetaGet().ctrl) {
      // cut to clipboard
      if (m_cursor != m_select) {
        EnvironmentGet()->ConfigurationGet().clipboard->Set(m_text.substr(std::min(m_cursor, m_select), std::abs(m_cursor - m_select)));

        // chop out that text
        int ncursor = std::min(m_cursor, m_select);
        TextSet(m_text.substr(0, std::min(m_cursor, m_select)) + m_text.substr(std::max(m_cursor, m_select)));
        CursorSet(ncursor);
        SelectionClear();
      }
    } else if (key == Input::Key::V && EnvironmentGet()->Input_MetaGet().ctrl) {
      // paste from clipboard
      std::string clipboard = EnvironmentGet()->ConfigurationGet().clipboard->Get();
      int ncursor = m_cursor + (int)clipboard.size();
      TextSet(m_text.substr(0, std::min(m_cursor, m_select)) + clipboard + m_text.substr(std::max(m_cursor, m_select)));
      CursorSet(ncursor);
      SelectionClear();
    }
  }

  /*static*/ int Text::luaF_SetText(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->TextSet(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Text::luaF_GetText(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushstring(L, self->TextGet().c_str());

    return 1;
  }

  /*static*/ int Text::luaF_SetFont(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->FontSet(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Text::luaF_GetFont(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushstring(L, self->GetFont().c_str());

    return 1;
  }

  /*static*/ int Text::luaF_SizeSet(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->SizeSet((float)luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Text::luaF_SizeGet(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->SizeGet());

    return 1;
  }

  /*static*/ int Text::luaF_SetWordwrap(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    luaL_checktype(L, 2, LUA_TBOOLEAN); // sigh
    self->WordwrapSet(lua_toboolean(L, 2) != 0);

    return 0;
  }

  /*static*/ int Text::luaF_GetWordwrap(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushboolean(L, self->WordwrapGet());

    return 1;
  }

  /*static*/ int Text::luaF_SetColor(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->ColorSet(Color((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4), (float)luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::luaF_GetColor(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    const Color &col = self->ColorGet();

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
      self->InteractiveSet(INTERACTIVE_NONE);
    } else if (strcmp(param, "select") == 0) {
      self->InteractiveSet(INTERACTIVE_SELECT);
    } else if (strcmp(param, "edit") == 0) {
      self->InteractiveSet(INTERACTIVE_EDIT);
    } else {
      luaL_error(L, "Invalid interaction type");
    }

    return 0;
  }

  /*static*/ int Text::luaF_GetInteractive(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    if (self->InteractiveGet() == INTERACTIVE_NONE) {
      lua_pushliteral(L, "none");
    } else if (self->InteractiveGet() == INTERACTIVE_SELECT) {
      lua_pushliteral(L, "select");
    } else if (self->InteractiveGet() == INTERACTIVE_EDIT) {
      lua_pushliteral(L, "edit");
    } else {
      self->EnvironmentGet()->LogError("Completely unknown interactivity flag, please report as a bug");
      lua_pushnil(L);
    }

    return 1;
  }

  /*static*/ int Text::luaF_SetCursor(lua_State *L) {
    luaF_checkparams(L, 2);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->CursorSet((int)luaL_checkinteger(L, 2));

    return 0;
  }

  /*static*/ int Text::luaF_GetCursor(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->CursorGet());

    return 1;
  }

  /*static*/ int Text::luaF_SetSelection(lua_State *L) {
    luaF_checkparams(L, 1, 3);
    Text *self = luaF_checkframe<Text>(L, 1);

    if (lua_gettop(L) == 1) {
      self->SelectionClear();
    } else if(lua_gettop(L) == 3) {
      self->SelectionSet((int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3));
    } else {
      luaL_error(L, "Incorrect number of parameters, expecting 1 or 3");
    }

    return 0;
  }

  /*static*/ int Text::luaF_GetSelection(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    int s, e;
    if (self->SelectionGet(&s, &e)) {
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

    self->ScrollSet(Vector((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3)));

    return 0;
  }

  /*static*/ int Text::luaF_GetScroll(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    lua_pushnumber(L, self->ScrollGet().x);
    lua_pushnumber(L, self->ScrollGet().y);

    return 2;
  }

  /*static*/ int Text::luaF_SetColorSelection(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->ColorSelectionSet(Color((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4), (float)luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::luaF_GetColorSelection(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    const Color &col = self->ColorSelectionGet();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }

  /*static*/ int Text::luaF_SetColorSelected(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Text *self = luaF_checkframe<Text>(L, 1);

    self->ColorSelectedSet(Color((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4), (float)luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Text::luaF_GetColorSelected(lua_State *L) {
    luaF_checkparams(L, 1);
    Text *self = luaF_checkframe<Text>(L, 1);

    const Color &col = self->ColorSelectedGet();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }
}

