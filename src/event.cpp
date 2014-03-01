
#include "frames/event.h"

namespace Frames {
  void Handle::luaF_push(lua_State *L) {
    // TODO: actually push something useful
    lua_pushnil(L);
  }
}
