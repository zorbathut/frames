
#include "frames/environment.h"
#include "frames/environment_register.h"

#include "frames/frame.h"
#include "frames/renderer.h"
#include "frames/text_manager.h"
#include "frames/texture_manager.h"

// these exist just for the lua init
#include "frames/text.h"
#include "frames/texture.h"
#include "frames/mask.h"

#include <GL/gl.h>

namespace Frames {
  Environment::Environment() {
    Configuration config;
    Init(config);
  }

  Environment::Environment(const Configuration &config) {
    Init(config);
  }

  Environment::~Environment() {
    m_root->Obliterate();

    // this flushes everything out of memory
    while (!m_invalidated.empty()) {
      const Layout *layout = m_invalidated.front();
      m_invalidated.pop_front();

      layout->Resolve();
    }

    delete m_text_manager;
    delete m_renderer;
    delete m_texture_manager;
    delete m_config_logger_owned;
  }

  void Environment::ResizeRoot(int x, int y) {
    m_root->SetWidth(x);
    m_root->SetHeight(y);
  }

  void Environment::Render(const Layout *root) {
    if (!root) {
      root = m_root;
    }

    if (root->GetEnvironment() != this) {
      GetConfiguration().logger->LogError("Attempt to render a frame through an unrelated environment");
      return;
    }

    // We want to batch up events if possible (todo: is this the case? which is faster - flushing as they go, or flushing all at once?) so, two nested loops
    while (!m_invalidated.empty()) {
      while (!m_invalidated.empty()) {
        const Layout *layout = m_invalidated.front();
        m_invalidated.pop_front();

        layout->Resolve();
      }

      // send events here
    }

    m_renderer->Begin(m_root->GetWidth(), m_root->GetHeight());

    root->Render(m_renderer);

    m_renderer->End();
  }

  void Environment::RegisterLua(lua_State *L) {
    LuaStackChecker(L, this);

    // insert our framespec metatable table
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_mt");
    if (lua_isnil(L, -1)) {
      lua_newtable(L);
      lua_setfield(L, LUA_REGISTRYINDEX, "Frames_mt");
    }
    lua_pop(L, 1);

    // insert our registry table - luatable to lightuserdata. Split up by type, so we can do type checking and handle MI pointer conversion. Maybe make this optional? I think MI can be handled differently.
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rg");
    if (lua_isnil(L, -1)) {
      lua_newtable(L);
      lua_setfield(L, LUA_REGISTRYINDEX, "Frames_rg");
    }
    lua_pop(L, 1);

    // insert our reverse registry table - lightuserdata to luatable. Does not need to be split up by type, we'll always be pushing it as a Layout*.
    lua_getfield(L, LUA_REGISTRYINDEX, "Frames_rrg");
    if (lua_isnil(L, -1)) {
      lua_newtable(L);
      lua_setfield(L, LUA_REGISTRYINDEX, "Frames_rrg");
    }
    lua_pop(L, 1);

    // insert our global table
    lua_getfield(L, LUA_GLOBALSINDEX, "Frames");
    if (lua_isnil(L, -1)) {
      lua_newtable(L);
      lua_setfield(L, LUA_GLOBALSINDEX, "Frames");
    }
    lua_pop(L, 1);

    // we'll need this for Root
    RegisterLuaFrame<Layout>(L);

    // And insert the Root member into the frames
    lua_getglobal(L, "Frames");

    m_root->l_push(L);
    lua_setfield(L, -2, "Root");

    lua_pop(L, 1);
  }

  void Environment::RegisterLuaFramesBuiltin(lua_State *L) {
    RegisterLuaFrameCreation<Frame>(L);
    RegisterLuaFrameCreation<Text>(L);
    RegisterLuaFrameCreation<Texture>(L);
    RegisterLuaFrameCreation<Mask>(L);
  }

  void Environment::Init(const Configuration &config) {
    m_config_logger_owned = 0;

    m_config = config;

    if (!m_config.logger) {
      m_config_logger_owned = new Configuration::Logger();
      m_config.logger = m_config_logger_owned;
    }

    if (!m_config.textureFromId) {
      m_config_tfi_owned = new Configuration::TextureFromId();
      m_config.textureFromId = m_config_tfi_owned;
    }

    if (!m_config.streamFromId) {
      m_config_sfi_owned = new Configuration::StreamFromId();
      m_config.streamFromId = m_config_sfi_owned;
    }

    if (!m_config.pathFromId) {
      m_config_pfi_owned = new Configuration::PathFromId();
      m_config.pathFromId = m_config_pfi_owned;
    }

    if (!m_config.textureFromStream) {
      m_config_tfs_owned = new Configuration::TextureFromStream();
      m_config.textureFromStream = m_config_tfs_owned;
    }

    m_root = new Layout(0, this);
    m_root->SetNameStatic("Root");

    m_renderer = new Renderer(this);
    m_text_manager = new TextManager(this);
    m_texture_manager = new TextureManager(this);
  }

  void Environment::MarkInvalidated(const Layout *layout) {
    m_invalidated.push_back(layout);
  }

  void Environment::LayoutStack_Push(const Layout *layout, Axis axis, float pt) {
    LayoutStack_Entry entry = {layout, axis, pt};
    m_layoutStack.push_back(entry);
  }

  void Environment::LayoutStack_Push(const Layout *layout, Axis axis) {
    LayoutStack_Entry entry = {layout, axis, Utility::Undefined};
    m_layoutStack.push_back(entry);
  }

  void Environment::LayoutStack_Pop() {
    m_layoutStack.pop_back();
  }

  void Environment::LayoutStack_Error() {
    if (m_layoutStack.empty()) {
      LogError("Layout loop dependency message received, but stack is empty. This should never happen.");
      return;
    }

    LogError(Utility::Format("Layout loop dependency detected, axis %c:", (m_layoutStack[0].axis == X) ? 'X' : 'Y'));
    for (int i = m_layoutStack.size(); i > 0; --i) {
      LayoutStack_Entry entry = m_layoutStack[i - 1];
      if (Utility::IsUndefined(entry.point))
        LogError(Utility::Format("  %s: size", entry.layout->GetNameFull().c_str()));
      else if (entry.point == 0 && entry.axis == X)
        LogError(Utility::Format("  %s: LEFT", entry.layout->GetNameFull().c_str()));
      else if (entry.point == 0 && entry.axis == Y)
        LogError(Utility::Format("  %s: TOP", entry.layout->GetNameFull().c_str()));
      else if (entry.point == 0.5)
        LogError(Utility::Format("  %s: CENTER", entry.layout->GetNameFull().c_str()));
      else if (entry.point == 1 && entry.axis == X)
        LogError(Utility::Format("  %s: RIGHT", entry.layout->GetNameFull().c_str()));
      else if (entry.point == 1 && entry.axis == Y)
        LogError(Utility::Format("  %s: BOTTOM", entry.layout->GetNameFull().c_str()));
      else
        LogError(Utility::Format("  %s: %f", entry.layout->GetNameFull().c_str(), entry.point));
    }
  }

  Environment::LuaStackChecker::LuaStackChecker(lua_State *L, Environment *env) : m_depth(lua_gettop(L)), m_L(L), m_env(env) {
  }
  Environment::LuaStackChecker::~LuaStackChecker() {
    if (m_depth != lua_gettop(m_L)) {
      m_env->LogError(Utility::Format("Lua stack size mismatch (%d -> %d)", m_depth, lua_gettop(m_L)));
    }
  }
}

