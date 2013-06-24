
#include "frames/frame.h"

#include "frames/environment.h"
#include "frames/lua.h"
#include "frames/renderer.h"

#include <GL/gl.h>

namespace Frame {
  Frame *Frame::CreateBare(Layout *parent) {
    return new Frame(parent);
  }
  Frame *Frame::CreateTagged_imp(const char *filename, int line, Layout *parent) {
    Frame *rv = new Frame(parent);
    rv->SetNameStatic(filename);
    rv->SetNameId(line);
    return rv;
  }

  /*static*/ const char *Frame::GetStaticType() {
    return "Frame";
  }

  void Frame::SetBackground(const Color &color) {
    if (color != m_bg) {
      m_bg = color;
    }
  }

  void Frame::SetFocus(bool focus) {
    if (focus != GetFocus()) {
      if (!focus) {
        GetEnvironment()->ClearFocus();
      } else {
        GetEnvironment()->SetFocus(this);
      }
    }
  }

  bool Frame::GetFocus() const {
    return GetEnvironment()->GetFocus() == this;
  }

  void Frame::RenderElement(Renderer *renderer) const {
    if (m_bg.a > 0) {
      Color bgc = m_bg * Color(1, 1, 1, renderer->AlphaGet());

      renderer->SetTexture();

      float u = GetTop();
      float d = GetBottom();
      float l = GetLeft();
      float r = GetRight();

      Renderer::Vertex *v = renderer->Request(4);

      v[0].p.x = l; v[0].p.y = u;
      v[1].p.x = r; v[1].p.y = u;
      v[2].p.x = r; v[2].p.y = d;
      v[3].p.x = l; v[3].p.y = d;

      v[0].c = bgc;
      v[1].c = bgc;
      v[2].c = bgc;
      v[3].c = bgc;

      renderer->Return(GL_QUADS);
    }
  }

  /*static*/ void Frame::luaF_RegisterFunctions(lua_State *L) {
    Layout::luaF_RegisterFunctions(L);

    luaF_RegisterFunction(L, GetStaticType(), "SetPoint", luaF_SetPoint);

    luaF_RegisterFunction(L, GetStaticType(), "SetWidth", luaF_SetWidth);
    luaF_RegisterFunction(L, GetStaticType(), "SetHeight", luaF_SetHeight);

    luaF_RegisterFunction(L, GetStaticType(), "SetParent", luaF_SetParent);
    luaF_RegisterFunction(L, GetStaticType(), "GetParent", luaF_GetParent);

    luaF_RegisterFunction(L, GetStaticType(), "SetLayer", luaF_SetLayer);
    luaF_RegisterFunction(L, GetStaticType(), "GetLayer", luaF_GetLayer);

    luaF_RegisterFunction(L, GetStaticType(), "SetStrata", luaF_SetStrata);
    luaF_RegisterFunction(L, GetStaticType(), "GetStrata", luaF_GetStrata);

    luaF_RegisterFunction(L, GetStaticType(), "SetVisible", luaF_SetVisible);
    luaF_RegisterFunction(L, GetStaticType(), "GetVisible", luaF_GetVisible);

    luaF_RegisterFunction(L, GetStaticType(), "SetAlpha", luaF_SetAlpha);
    luaF_RegisterFunction(L, GetStaticType(), "GetAlpha", luaF_GetAlpha);

    luaF_RegisterFunction(L, GetStaticType(), "SetBackground", luaF_SetBackground);
    luaF_RegisterFunction(L, GetStaticType(), "GetBackground", luaF_GetBackground);

    luaF_RegisterFunction(L, GetStaticType(), "SetFocus", luaF_SetFocus);
    luaF_RegisterFunction(L, GetStaticType(), "GetFocus", luaF_GetFocus);

    luaF_RegisterFunction(L, GetStaticType(), "Obliterate", luaF_Obliterate);
  }

  Frame::Frame(Layout *parent) :
      Layout(parent),
      m_bg(0, 0, 0, 0)
  { };
  Frame::~Frame() { };

  static void luaF_ParseCoord(lua_State *L, int *cindex, bool stringable, bool write, bool *xe, bool *ye, float *xt, float *yt) {
    bool lxe = false;
    bool lye = false;

    if (lua_gettop(L) < *cindex) {
      luaL_error(L, "Ran out of parameters in SetPoint");
    }

    if (stringable && lua_type(L, *cindex) == LUA_TSTRING) {
      const char *str = lua_tostring(L, *cindex);

      // this could maybe be done more efficiently, but I have a hard time imagining a sane case where this is a speed bottleneck
      if (strcmp(str, "LEFT") == 0) {
        lxe = true; *xt = 0.f;
      } else if (strcmp(str, "RIGHT") == 0) {
        lxe = true; *xt = 1.f;
      } else if (strcmp(str, "TOP") == 0) {
        lye = true; *yt = 0.f;
      } else if (strcmp(str, "BOTTOM") == 0) {
        lye = true; *yt = 1.f;
      } else if (strcmp(str, "TOPLEFT") == 0 || strcmp(str, "LEFTTOP") == 0) {
        lxe = true; lye = true; *xt = 0.f; *yt = 0.f;
      } else if (strcmp(str, "TOPCENTER") == 0 || strcmp(str, "CENTERTOP") == 0) {
        lxe = true; lye = true; *xt = 0.5f; *yt = 0.f;
      } else if (strcmp(str, "TOPRIGHT") == 0 || strcmp(str, "RIGHTTOP") == 0) {
        lxe = true; lye = true; *xt = 1.f; *yt = 0.f;
      } else if (strcmp(str, "CENTERLEFT") == 0 || strcmp(str, "LEFTCENTER") == 0) {
        lxe = true; lye = true; *xt = 0.f; *yt = 0.5f;
      } else if (strcmp(str, "CENTER") == 0 || strcmp(str, "CENTERCENTER") == 0) {
        lxe = true; lye = true; *xt = 0.5f; *yt = 0.5f;
      } else if (strcmp(str, "CENTERRIGHT") == 0 || strcmp(str, "RIGHTCENTER") == 0) {
        lxe = true; lye = true; *xt = 1.f; *yt = 0.5f;
      } else if (strcmp(str, "BOTTOMLEFT") == 0 || strcmp(str, "LEFTBOTTOM") == 0) {
        lxe = true; lye = true; *xt = 0.f; *yt = 1.f;
      } else if (strcmp(str, "BOTTOMCENTER") == 0 || strcmp(str, "CENTERBOTTOM") == 0) {
        lxe = true; lye = true; *xt = 0.5f; *yt = 1.f;
      } else if (strcmp(str, "BOTTOMRIGHT") == 0 || strcmp(str, "RIGHTBOTTOM") == 0) {
        lxe = true; lye = true; *xt = 1.f; *yt = 1.f;
      } else {
        luaL_error(L, "Unrecognized position string %s", str);
      }

      (*cindex)++;
    } else {
      if (lua_gettop(L) < *cindex + 1) {
        luaL_error(L, "Ran out of parameters in SetPoint");
      }

      int tx = lua_type(L, *cindex);
      int ty = lua_type(L, *cindex + 1);

      if ((tx != LUA_TNUMBER && tx != LUA_TNIL) || (ty != LUA_TNUMBER && ty != LUA_TNIL)) {
        luaL_error(L, "Invalid parameter type in SetPoint");
      }
      if (tx == LUA_TNIL && ty == LUA_TNIL) {
        luaL_error(L, "No actual coordinates in SetPoint");
      }

      lxe = (tx == LUA_TNUMBER);
      lye = (ty == LUA_TNUMBER);

      *xt = lua_tonumber(L, *cindex);
      *yt = lua_tonumber(L, *cindex + 1);

      (*cindex) += 2;
    }

    if (write) {
      *xe = lxe;
      *ye = lye;
    } else {
      if (*xe != lxe || *ye != lye) {
        luaL_error(L, "Inconsistent nils in SetPoint");
      }
    }
  }

  /*static*/ int Frame::luaF_SetPoint(lua_State *L) {
    // this one is a horrible beast
    Frame *self = luaF_checkframe<Frame>(L, 1);

    int cindex = 2;

    // horrible
    // horrrrrrible
    bool x_enabled = false;
    bool y_enabled = false;
    float x_src = 0.f;
    float y_src = 0.f;

    luaF_ParseCoord(L, &cindex, true, true, &x_enabled, &y_enabled, &x_src, &y_src);

    if (lua_gettop(L) < cindex) {
      luaL_error(L, "Ran out of parameters in SetPoint");
    }

    Layout *target = 0;
    float x_target = 0.f;
    float y_target = 0.f;
    if (lua_type(L, cindex) == LUA_TSTRING && strcmp(lua_tostring(L, cindex), "origin") == 0) {
      // well okay I guess that's it
      cindex++;
    } else {
      target = luaF_checkframe_external<Layout>(L, cindex);
      cindex++;

      luaF_ParseCoord(L, &cindex, true, false, &x_enabled, &y_enabled, &x_target, &y_target);
    }

    float x_ofs = 0.f;
    float y_ofs = 0.f;
    if (lua_gettop(L) >= cindex) {
      luaF_ParseCoord(L, &cindex, false, false, &x_enabled, &y_enabled, &x_ofs, &y_ofs);
    }

    if (lua_gettop(L) != cindex - 1) {
      luaL_error(L, "Too many parameters in SetPoint");
    }

    if (x_enabled) {
      self->SetPoint(X, x_src, target, x_target, x_ofs);
    }
    if (y_enabled) {
      self->SetPoint(Y, y_src, target, y_target, y_ofs);
    }

    return 0;
  }

  /*static*/ int Frame::luaF_SetWidth(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetWidth(luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_SetHeight(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetHeight(luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_SetParent(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetParent(luaF_checkframe<Layout>(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_GetParent(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->GetParent()->luaF_push(L);

    return 1;
  }

  /*static*/ int Frame::luaF_SetLayer(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetLayer(luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_GetLayer(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushnumber(L, self->GetLayer());

    return 1;
  }

  /*static*/ int Frame::luaF_SetStrata(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetStrata(luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_GetStrata(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushnumber(L, self->GetStrata());

    return 1;
  }

  /*static*/ int Frame::luaF_SetVisible(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    luaL_checktype(L, 2, LUA_TBOOLEAN); // sigh
    self->SetVisible(lua_toboolean(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_GetVisible(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushboolean(L, self->GetVisible());

    return 1;
  }

  /*static*/ int Frame::luaF_SetAlpha(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetAlpha(luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_GetAlpha(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushnumber(L, self->GetAlpha());

    return 1;
  }

  /*static*/ int Frame::luaF_SetBackground(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetBackground(Color(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_optnumber(L, 5, 1.f)));

    return 0;
  }

  /*static*/ int Frame::luaF_GetBackground(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    const Color &col = self->GetBackground();

    lua_pushnumber(L, col.r);
    lua_pushnumber(L, col.g);
    lua_pushnumber(L, col.b);
    lua_pushnumber(L, col.a);

    return 4;
  }

  /*static*/ int Frame::luaF_SetFocus(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetFocus(lua_toboolean(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_GetFocus(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushboolean(L, self->GetFocus());

    return 1;
  }

  /*static*/ int Frame::luaF_Obliterate(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->Obliterate();

    return 0;
  }
}

