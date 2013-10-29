 // The Environment registration

#ifndef FRAME_ENVIRONMENT_REGISTER
#define FRAME_ENVIRONMENT_REGISTER

#include "frame/environment.h"

#include "os_lua.h"

#include <windows.h>

namespace Frame {
  // Template creation function. Not really part of environment, and there are difficulties with taking the address of member function templates on many semi-modern compilers
  // So we're just going it this way
  template <typename T> int luaF_frameCreate(lua_State *L) {
    if (lua_gettop(L) != 1) luaL_error(L, "Wrong number of parameters to Frame creation function (%d, should be %d)", lua_gettop(L), 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    // We need to do a lookup to ensure this is actually a layout
    lua_rawget(L, lua_upvalueindex(1));

    if (lua_isnil(L, 1)) luaL_error(L, "Parameter to frame creation function does not seem to be a valid layout");
    if (!lua_isuserdata(L, 1)) luaL_error(L, "Internal error, item is not a userdata");

    Layout *layout = (Layout *)lua_touserdata(L, 1);

    // now we have a valid layout

    T *item = T::CreateBare(layout);

    // Optional tagging with file/line
    {
      std::string id;

      lua_Debug ar;
      if (lua_getstack(L, 1, &ar)) {
        lua_getinfo(L, "Snl", &ar);
        id += Utility::Format("%s:", ar.short_src);
        if (ar.currentline > 0)
          id += Utility::Format("%d:", ar.currentline);
        if (*ar.namewhat != '\0') {
          id += Utility::Format(" in function %s", ar.name);
        } else {
          if (*ar.what == 'm') {
            id += " in main chunk";
          } else if (*ar.what == 'C' || *ar.what == 't') {
            id += " ?";
          } else {
            id += Utility::Format(" in function <%s:%d>", ar.short_src, ar.linedefined);
          }
        }
      } else {
        id = "(unknown lua)";
      }

      item->SetNameDynamic(id);
    }

    // Ahoy!

    item->luaF_push(L);

    return 1;
  }

  template <typename T> void Environment::LuaRegisterFrameLookup(lua_State *L) {
    LuaStackChecker lsc(L, this);

    // First, we need to see if we even need to do all this registration foofery
    // Yeah, that's right, I called it "foofery"
    lua_getfield(L, LUA_REGISTRYINDEX, "Frame_mt");
    if (lua_isnil(L, -1)) {
      LogError("Frame not yet registered to Lua environment");
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

    // Stack: ... Frame_mt

    // Make the RG and RRG tables
    // There are bits in here that could theoretically be done more efficiently, but it's not worth the added complexity to save a few cycles here
    
    // Pull out the RG
    lua_getfield(L, LUA_REGISTRYINDEX, "Frame_rg");
    if (lua_isnil(L, -1)) {
      LogError("Frame not yet registered to Lua environment");
      lua_pop(L, 2);
      return;
    }

    // Stack: ... Frame_mt Frame_rg
    
    // See if we've already got a registry table. Used for correlating Lua tables with lightuserdata, we don't need to put anything in it yet
    lua_getfield(L, -1, T::GetStaticType());
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);

      lua_newtable(L);

      // TODO: don't include this if we're in release mode
      lua_pushstring(L, T::GetStaticType());
      lua_setfield(L, -2, "id");

      lua_setfield(L, -2, T::GetStaticType());
    } else {
      LogError("Registry table already exists, but the metatable didn't? This should not happen ever");
      lua_pop(L, 1);
    }

    // Pull out the various event tables
    lua_getfield(L, LUA_REGISTRYINDEX, "Frame_fevh");
    if (lua_isnil(L, -1)) {
      LogError("Frame not yet registered to Lua environment");
      lua_pop(L, 3);
      return;
    }

    lua_getfield(L, LUA_REGISTRYINDEX, "Frame_rfevh");
    if (lua_isnil(L, -1)) {
      LogError("Frame not yet registered to Lua environment");
      lua_pop(L, 4);
      return;
    }

    lua_getfield(L, LUA_REGISTRYINDEX, "Frame_cfevh");
    if (lua_isnil(L, -1)) {
      LogError("Frame not yet registered to Lua environment");
      lua_pop(L, 5);
      return;
    }

    // Stack: ... Frame_mt Frame_rg Frame_fevh Frame_rfevh Frame_cfevh

    // Now it's time to rig up the actual metatable that we plan to create

    // This will be our metatable
    lua_newtable(L);

    // This will be our function lookup
    lua_newtable(L);

    // Stack: ... Frame_mt Frame_rg Frame_fevh Frame_rfevh Frame_cfevh metatable indexes
    // This goes back to Layout::luaF_RegisterFunction - if this layout is changed, that function may need to be changed
    // luaF_RegisterFunctions used to want a lot of these parameters so it could efficiently set up upvalues. Right now, we don't really care. This can probably be cleaned up quite a bit.
    T::luaF_RegisterFunctions(L);

    // Attach it as the index accessor
    lua_setfield(L, -2, "__index");

    // Stack: ... Frame_mt Frame_rg Frame_fevh Frame_rfevh Frame_cfevh metatable

    // Register the metatable
    lua_setfield(L, -6, T::GetStaticType());

    // Stack: ... Frame_mt Frame_rg Frame_fevh Frame_rfevh Frame_cfevh

    lua_pop(L, 5);
  }

  template <typename T> void Environment::LuaRegisterFrame(lua_State *L) {
    LuaStackChecker lsc(L, this);

    LuaRegisterFrameLookup<T>(L);

    // We need to insert our creation function
    lua_getglobal(L, "Frame");
    if (lua_isnil(L, -1)) {
      LogError("Frame not yet registered to Lua environment");
      lua_pop(L, 1);
      return;
    }

    // Stack: ... globalFrame

    lua_getfield(L, -1, T::GetStaticType());
    if (!lua_isnil(L, -1)) {
      // well okay I guess we're done
      lua_pop(L, 2);
      return;
    }

    // Dump the nil
    lua_pop(L, 1);

    // Stack: ... globalFrame
    
    // Add the upvalue parameter

    // Pull out the RG
    lua_getfield(L, LUA_REGISTRYINDEX, "Frame_rg");
    if (lua_isnil(L, -1)) {
      LogError("Frame not yet registered to Lua environment");
      lua_pop(L, 2);
      return;
    }

    lua_getfield(L, -1, "Layout");
    if (lua_isnil(L, -1)) {
      LogError("Cannot find Layout lookup when registering, extremely confused");
      lua_pop(L, 3);
      return;
    }

    // Stack: ... globalFrame Frame_rg Layoutlookup

    // Insert function
    lua_pushcclosure(L, &luaF_frameCreate<T>, 1);
    lua_setfield(L, -3, T::GetStaticType());

    // Stack: ... globalFrame Frame_rg

    // Clean up and we're done
    lua_pop(L, 2);
  }
}

#endif
      
