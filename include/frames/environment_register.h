 // The Environment registration

#ifndef FRAMES_ENVIRONMENT_REGISTER
#define FRAMES_ENVIRONMENT_REGISTER

#include "frames/environment.h"

#include "lua.hpp"

namespace Frames {
  template<typename T> void Environment::RegisterLuaFrame(lua_State *L) {
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
      lua_setfield(L, -2, T::GetStaticType());
    } else {
      LogError("Registry table already exists? This should not happen ever");
      lua_pop(L, 1);
    }

    // Pull out the RRG
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rrg");
    if (lua_isnil(L, -1)) {
      LogError("Frames not yet registered to Lua environment");
      lua_pop(L, 2);
      return;
    }

    // Stack: ... Frames_mt Frames_rg Frames_rrg
    
    // See if we've already got a registry table
    lua_getfield(L, -1, T::GetStaticType());
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);

      lua_newtable(L);
      lua_setfield(L, -2, T::GetStaticType());
    } else {
      LogError("Reverse registry table already exists? This should not happen ever");
      lua_pop(L, 1);
    }

    // Now it's time to rig up the actual metatable that we plan to create

    // This will be our metatable
    lua_newtable(L);

    // This will be our function lookup
    lua_newtable(L);

    // Insert functions here

    // Attach it as the index accessor
    lua_setfield(L, -2, "__index");

    // Stack: ... Frames_mt Frames_rg Frames_rrg metatable

    // Register the metatable
    lua_setfield(L, -4, T::GetStaticType());

    // Stack: ... Frames_mt Frames_rg Frames_rrg

    // Now we need to insert our creation function        
    lua_getglobal(L, "Frames");
    if (lua_isnil(L, -1)) {
      LogError("Frames not yet registered to Lua environment");
      lua_pop(L, 3);
      return;
    }

    // Stack: ... Frames_mt Frames_rg Frames_rrg functiontable
    
    // TODO: insert the function instead, with attached data if needed
    lua_pushboolean(L, true);
    lua_setfield(L, -2, T::GetStaticType());

    // Stack: ... Frames_mt Frames_rg Frames_rrg functiontable

    // Clean up and we're done
    lua_pop(L, 4);
  }
}

#endif
      
