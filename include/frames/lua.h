// Lua binding utilities

#ifndef FRAMES_LUA
#define FRAMES_LUA

#include "os_lua.h"
#include "input.h"

#include <string>

namespace Frames {
  struct Vector;
  class VerbBase;
  
  // luaF_push is intended to be extended by end users! Add more overloads as you see fit to conform to your event schema.
  void luaF_push(lua_State *L, int x);
  void luaF_push(lua_State *L, double x);
  void luaF_push(lua_State *L, const std::string &x);
  void luaF_push(lua_State *L, const Vector &pt);
  void luaF_push(lua_State *L, const Input::Key &kev);
  
  template <typename T> T *luaF_checkframe_fromregistry(lua_State *L, int index, int registry) {
    luaL_checktype(L, index, LUA_TTABLE);
    lua_pushvalue(L, index);
    lua_rawget(L, registry);

    if (lua_isnil(L, -1)) {
      luaL_error(L, "Failed to find frame of type %s for parameter %d", T::TypeStaticGet(), index);
    }

    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    T *result = (T*)lua_touserdata(L, -1);

    lua_pop(L, 1);

    return result;
  }

  template <typename T> T *luaF_checkframe(lua_State *L, int index) {
    if (index < 0) index += lua_gettop(L) + 1;

    // TODO: remove this doublecheck in optimized mode
    luaL_checktype(L, lua_upvalueindex(1), LUA_TTABLE);
    lua_getfield(L, lua_upvalueindex(1), "id");
    const char *res = luaL_checklstring(L, -1, NULL);
    if (strcmp(res, T::TypeStaticGet()) != 0) {
      luaL_error(L, "Internal error, mismatch in expected registry table %s against actual table %s", T::TypeStaticGet(), res);
    }
    lua_pop(L, 1);

    return luaF_checkframe_fromregistry<T>(L, index, lua_upvalueindex(1));
  }

  template <typename T> T *luaF_checkframe_external(lua_State *L, int index) {
    if (index < 0) index += lua_gettop(L) + 1;

    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rg");
    lua_getfield(L, -1, T::TypeStaticGet());

    T *result = luaF_checkframe_fromregistry<T>(L, index, lua_gettop(L));

    lua_pop(L, 2); // get rid of the lookup table and friend

    return result;
  }

  inline void luaF_checkparams(lua_State *L, int mn, int mx = -1) {
    if (mx == -1) mx = mn;
    int lgt = lua_gettop(L);
    if (lgt < mn || lgt > mx) {
      luaL_error(L, "Incorrect number of parameters %d (expecting between %d and %d)", lgt, mn, mx);
    }
  }
  
  inline const VerbBase *luaF_checkevent(lua_State *L, int index) {
    if (index < 0) index += lua_gettop(L) + 1;
    
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_fev");
    lua_pushvalue(L, index);
    lua_rawget(L, -2);
    
    const VerbBase *ebf = (const VerbBase *)lua_touserdata(L, -1);
    
    if (!ebf) {
      luaL_error(L, "Not a valid frame event handle");
    }
    
    lua_pop(L, 2);
    
    return ebf;
  }
}

#endif
