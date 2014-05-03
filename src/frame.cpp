
#include "frames/frame.h"

#include "frames/cast.h"
#include "frames/environment.h"
#include "frames/lua.h"
#include "frames/renderer.h"

#include <GL/gl.h>

namespace Frames {
  FRAMES_DEFINE_RTTI(Frame, Layout);

  Frame *Frame::Create(const std::string &name, Layout *parent) {
    return new Frame(name, parent);
  }

  void Frame::SetBackground(const Color &color) {
    if (color != m_bg) {
      m_bg = color;
    }
  }

  void Frame::RenderElement(detail::Renderer *renderer) const {
    if (m_bg.a > 0) {
      Color bgc = m_bg * Color(1, 1, 1, renderer->AlphaGet());

      renderer->SetTexture();

      float u = TopGet();
      float d = BottomGet();
      float l = LeftGet();
      float r = RightGet();

      detail::Renderer::Vertex *v = renderer->Request(4);

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

    luaF_RegisterFunction(L, TypeStaticGet(), "PinSet", luaF_PinSet);

    luaF_RegisterFunction(L, TypeStaticGet(), "WidthSet", luaF_WidthSet);
    luaF_RegisterFunction(L, TypeStaticGet(), "HeightSet", luaF_HeightSet);

    luaF_RegisterFunction(L, TypeStaticGet(), "ParentSet", luaF_ParentSet);
    luaF_RegisterFunction(L, TypeStaticGet(), "ParentGet", luaF_ParentGet);

    luaF_RegisterFunction(L, TypeStaticGet(), "LayerSet", luaF_LayerSet);
    luaF_RegisterFunction(L, TypeStaticGet(), "LayerGet", luaF_LayerGet);

    luaF_RegisterFunction(L, TypeStaticGet(), "ImplementationSet", luaF_ImplementationSet);
    luaF_RegisterFunction(L, TypeStaticGet(), "ImplementationGet", luaF_ImplementationGet);

    luaF_RegisterFunction(L, TypeStaticGet(), "VisibleSet", luaF_VisibleSet);
    luaF_RegisterFunction(L, TypeStaticGet(), "VisibleGet", luaF_VisibleGet);

    luaF_RegisterFunction(L, TypeStaticGet(), "AlphaSet", luaF_AlphaSet);
    luaF_RegisterFunction(L, TypeStaticGet(), "AlphaGet", luaF_AlphaGet);

    luaF_RegisterFunction(L, TypeStaticGet(), "SetBackground", luaF_SetBackground);
    luaF_RegisterFunction(L, TypeStaticGet(), "GetBackground", luaF_GetBackground);

    luaF_RegisterFunction(L, TypeStaticGet(), "Obliterate", luaF_Obliterate);
  }

  Frame::Frame(const std::string &name, Layout *parent) :
      Layout(name, parent->EnvironmentGet()),
      m_bg(0, 0, 0, 0)
  {
    ParentSet(parent);
  }
  Frame::~Frame() { }

  static void luaF_ParseCoord(lua_State *L, int *cindex, bool stringable, bool write, bool *xe, bool *ye, float *xt, float *yt) {
    bool lxe = false;
    bool lye = false;

    if (lua_gettop(L) < *cindex) {
      luaL_error(L, "Ran out of parameters in PinSet");
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
        luaL_error(L, "Ran out of parameters in PinSet");
      }

      int tx = lua_type(L, *cindex);
      int ty = lua_type(L, *cindex + 1);

      if ((tx != LUA_TNUMBER && tx != LUA_TNIL) || (ty != LUA_TNUMBER && ty != LUA_TNIL)) {
        luaL_error(L, "Invalid parameter type in PinSet");
      }
      if (tx == LUA_TNIL && ty == LUA_TNIL) {
        luaL_error(L, "No actual coordinates in PinSet");
      }

      lxe = (tx == LUA_TNUMBER);
      lye = (ty == LUA_TNUMBER);

      *xt = (float)lua_tonumber(L, *cindex);
      *yt = (float)lua_tonumber(L, *cindex + 1);

      (*cindex) += 2;
    }

    if (write) {
      *xe = lxe;
      *ye = lye;
    } else {
      if (*xe != lxe || *ye != lye) {
        luaL_error(L, "Inconsistent nils in PinSet");
      }
    }
  }

  /*static*/ int Frame::luaF_PinSet(lua_State *L) {
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
      luaL_error(L, "Ran out of parameters in PinSet");
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
      luaL_error(L, "Too many parameters in PinSet");
    }

    if (x_enabled) {
      self->PinSet(X, x_src, target, x_target, x_ofs);
    }
    if (y_enabled) {
      self->PinSet(Y, y_src, target, y_target, y_ofs);
    }

    return 0;
  }

  /*static*/ int Frame::luaF_WidthSet(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->WidthSet((float)luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_HeightSet(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->HeightSet((float)luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_ParentSet(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->ParentSet(luaF_checkframe<Layout>(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_ParentGet(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->ParentGet()->luaF_push(L);

    return 1;
  }

  /*static*/ int Frame::luaF_LayerSet(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->LayerSet((float)luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_LayerGet(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushnumber(L, self->LayerGet());

    return 1;
  }

  /*static*/ int Frame::luaF_ImplementationSet(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    luaL_checktype(L, 2, LUA_TBOOLEAN); // sigh
    self->ImplementationSet(lua_toboolean(L, 2) != 0);

    return 0;
  }

  /*static*/ int Frame::luaF_ImplementationGet(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushboolean(L, self->ImplementationGet());

    return 1;
  }

  /*static*/ int Frame::luaF_VisibleSet(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    luaL_checktype(L, 2, LUA_TBOOLEAN); // sigh
    self->VisibleSet(lua_toboolean(L, 2) != 0);

    return 0;
  }

  /*static*/ int Frame::luaF_VisibleGet(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushboolean(L, self->VisibleGet());

    return 1;
  }

  /*static*/ int Frame::luaF_AlphaSet(lua_State *L) {
    luaF_checkparams(L, 2);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->AlphaSet((float)luaL_checknumber(L, 2));

    return 0;
  }

  /*static*/ int Frame::luaF_AlphaGet(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    lua_pushnumber(L, self->AlphaGet());

    return 1;
  }

  /*static*/ int Frame::luaF_SetBackground(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->SetBackground(Color((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4), (float)luaL_optnumber(L, 5, 1.f)));

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

  /*static*/ int Frame::luaF_Obliterate(lua_State *L) {
    luaF_checkparams(L, 1);
    Frame *self = luaF_checkframe<Frame>(L, 1);

    self->Obliterate();

    return 0;
  }
}

