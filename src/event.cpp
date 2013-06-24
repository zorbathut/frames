
#include "frames/event.h"

namespace Frame {
  void EventHandle::luaF_push(lua_State *L) {
    // TODO: actually push something useful
    lua_pushnil(L);
  }
}
