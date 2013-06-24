
#include "frames/lua.h"

#include "frames/point.h"
#include "frames/input.h"

namespace Frames {
  void luaF_push(lua_State *L, int x) {
    lua_pushinteger(L, x);
  }
  void luaF_push(lua_State *L, double x) {
    lua_pushnumber(L, x);
  }
  void luaF_push(lua_State *L, const std::string &x) {
    lua_pushlstring(L, x.c_str(), x.size());
  }
  void luaF_push(lua_State *L, const Point &pt) {
    lua_pushnumber(L, pt.x);
    lua_pushnumber(L, pt.y);
  }
  void luaF_push(lua_State *L, const KeyEvent &kev) {
    lua_pushstring(L, Key::StringFromKey(kev.key));
    lua_newtable(L);
    if (kev.shift) {
      lua_pushboolean(L, true);
      lua_setfield(L, -2, "shift");
    }
    if (kev.alt) {
      lua_pushboolean(L, true);
      lua_setfield(L, -2, "alt");
    }
    if (kev.ctrl) {
      lua_pushboolean(L, true);
      lua_setfield(L, -2, "ctrl");
    }
  }
}
