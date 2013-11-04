// Including Lua is annoyingly tricky because of MSVC's slightly broken include path logic.
// As a result, this is a near-clone of lua.hpp.

#ifndef FRAMES_OS_LUA
#define FRAMES_OS_LUA

extern "C" {
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

#endif
