
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
  }

  Text::Text(Layout *parent) :
      Frame(parent),
      m_size(16),
      m_wordwrap(false),
      m_color_text(1, 1, 1)
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
    EventSizeAttach(Delegate<void ()>(this, &Text::UpdateLayout));
  };

  Text::~Text() { };
  
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
    }
  }

  void Text::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // we'll fix this up further later
    if (m_layout) {
      // do stuf
      m_layout->Render(renderer, m_color_text, GetBounds());
    }
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
}

