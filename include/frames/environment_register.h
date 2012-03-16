 // The Environment registration

#ifndef FRAMES_ENVIRONMENT_REGISTER
#define FRAMES_ENVIRONMENT_REGISTER

#include "frames/environment.h"

#include "lua.hpp"

#include <windows.h>

namespace Frames {
  // Template creation function. Not really part of environment, and there are difficulties with taking the address of member function templates on many semi-modern compilers
  // So we're just going it this way
  template <typename T> int l_create(lua_State *L) {
    if (lua_gettop(L) != 1) luaL_error(L, "Wrong number of parameters to Frames creation function (%d, should be %d)", lua_gettop(L), 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    // We need to do a lookup to ensure this is actually a layout
    lua_rawget(L, lua_upvalueindex(1));

    if (lua_isnil(L, 1)) luaL_error(L, "Parameter to frames creation function does not seem to be a valid layout");
    if (!lua_isuserdata(L, 1)) luaL_error(L, "Internal error, item is not a userdata");

    Layout *layout = (Layout *)lua_touserdata(L, 1);

    // now we have a valid layout

    T *item = T::CreateBare(layout);

    // Ahoy!

    item->l_push(L);

    return 1;
  }

  // This is the lookup infrastructure only, used in cases where 
  template <typename T> void Environment::RegisterLuaFrame(lua_State *L) {
    LuaStackChecker lsc(L, this);

    // First, we need to see if we even need to do all this registration foofery
    // Yeah, that's right, I called it "foofery"
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_mt");
    if (lua_isnil(L, -1)) {
      LogError("Frames not yet registered to Lua environment");
      lua_pop(L, 1);
      return;
    }

    lua_getfield(L, -1, T::GetStaticType());
    if (!lua_isnil(L, -1)) {
      // In Bizarro world, frame am already registered!
      lua_pop(L, 2);
      return;
    }

    // toss the nil, we don't need it
    lua_pop(L, 1);

    // Stack: ... Frames_mt

    // Make the RG and RRG tables
    // There are bits in here that could theoretically be done more efficiently, but it's not worth the added complexity to save a few cycles here
    
    // Pull out the RG
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rg");
    if (lua_isnil(L, -1)) {
      LogError("Frames not yet registered to Lua environment");
      lua_pop(L, 2);
      return;
    }

    // Stack: ... Frames_mt Frames_rg
    
    // See if we've already got a registry table
    lua_getfield(L, -1, T::GetStaticType());
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);

      lua_newtable(L);

      // TODO: don't include this if we're in release mode
      lua_pushstring(L, T::GetStaticType());
      lua_setfield(L, -2, "id");

      lua_setfield(L, -2, T::GetStaticType());
    } else {
      LogError("Registry table already exists? This should not happen ever");
      lua_pop(L, 1);
    }

    // Now it's time to rig up the actual metatable that we plan to create

    // This will be our metatable
    lua_newtable(L);

    // This will be our function lookup
    lua_newtable(L);

    // Stack: ... Frames_mt Frames_rg metatable indexes
    // This goes back to Layout::l_RegisterFunction - if this layout is changed, that function may need to be changed
    T::l_RegisterFunctions(L);

    // Attach it as the index accessor
    lua_setfield(L, -2, "__index");

    // Stack: ... Frames_mt Frames_rg metatable

    // Register the metatable
    lua_setfield(L, -3, T::GetStaticType());

    // Stack: ... Frames_mt Frames_rg

    lua_pop(L, 2);
  }

  template <typename T> void Environment::RegisterLuaFrameCreation(lua_State *L) {
    LuaStackChecker lsc(L, this);

    RegisterLuaFrame<T>(L);

    // We need to insert our creation function
    lua_getglobal(L, "Frames");
    if (lua_isnil(L, -1)) {
      LogError("Frames not yet registered to Lua environment");
      lua_pop(L, 1);
      return;
    }

    // Stack: ... globalFrames

    lua_getfield(L, -1, T::GetStaticType());
    if (!lua_isnil(L, -1)) {
      // well okay I guess we're done
      lua_pop(L, 2);
      return;
    }

    // Dump the nil
    lua_pop(L, 1);

    // Stack: ... globalFrames
    
    // Add the upvalue parameter

    // Pull out the RG
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rg");
    if (lua_isnil(L, -1)) {
      LogError("Frames not yet registered to Lua environment");
      lua_pop(L, 2);
      return;
    }

    lua_getfield(L, -1, "Layout");
    if (lua_isnil(L, -1)) {
      LogError("Cannot find Layout lookup when registering, extremely confused");
      lua_pop(L, 3);
      return;
    }

    // Stack: ... globalFrames Frames_rg Layoutlookup

    // Insert function
    lua_pushcclosure(L, &l_create<T>, 1);
    lua_setfield(L, -3, T::GetStaticType());

    // Stack: ... globalFrames Frames_rg

    // Clean up and we're done
    lua_pop(L, 2);
  }
}

#endif
      
