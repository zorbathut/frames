
#include "frames/event.h"

namespace Frames {
  void EventHandle::luaF_push(lua_State *L) {
    // TODO: actually push something useful
    lua_pushnil(L);
  }
}
