// The Environment class

#ifndef FRAMES_ENVIRONMENT
#define FRAMES_ENVIRONMENT

#include "frames/configuration.h"
#include "frames/noncopyable.h"
#include "frames/utility.h"

#include <deque>
#include <vector>
#include <set>

struct lua_State;

namespace Frames {
  class Frame;
  class Layout;
  class Renderer;
  class TextManager;
  class TextureManager;

  class Environment : Noncopyable {
  public:
    Environment();  // init to default
    Environment(const Configuration &config);
    ~Environment();

    // Update to the state of the world, incoming events
    void ResizeRoot(int x, int y);

    void MouseMove(int x, int y);
    void MouseDown(int button);
    void MouseUp(int button);
    void MouseWheel(int delta);

    void MouseClear();  // mouse no longer in the scene at all
    
    // Rendering
    void Render(const Layout *root = 0);
    
    // Introspection
    Layout *GetRoot() { return m_root; }
    Layout *GetFrameUnder(int x, int y);

    const Configuration &GetConfiguration() { return m_config; }

    // Scripting languages
    void LuaRegister(lua_State *L);
    void LuaUnregister(lua_State *L);
    void LuaRegisterFramesBuiltin(lua_State *L);
    template<typename T> void RegisterLuaFrame(lua_State *L);
    template<typename T> void RegisterLuaFrameCreation(lua_State *L); // Calls RegisterLuaFrame implicitly

    // Internal only, do not call below this line
    TextManager *GetTextManager() { return m_text_manager; }
    TextureManager *GetTextureManager() { return m_texture_manager; }

    void LogError(const std::string &log) { if (m_config.logger) m_config.logger->LogError(log); }
    void LogDebug(const std::string &log) { if (m_config.logger) m_config.logger->LogDebug(log); }
  private:
    friend class Layout;

    // Utility functions and parameters
    void Init(const Configuration &config);

    void MarkInvalidated(const Layout *layout);
    std::deque<const Layout *> m_invalidated;

    // Layout sanity
    void LayoutStack_Push(const Layout *layout, Axis axis, float pt);
    void LayoutStack_Push(const Layout *layout, Axis axis);
    void LayoutStack_Pop();
    void LayoutStack_Error();
    struct LayoutStack_Entry {
      const Layout *layout;
      Axis axis;
      float point;
    };
    std::vector<LayoutStack_Entry> m_layoutStack;
    
    // Maintenance
    void DestroyingLayout(Layout *layout);

    // Configuration
    Configuration m_config;
    Configuration::Logger *m_config_logger_owned;
    Configuration::TextureFromId *m_config_tfi_owned;
    Configuration::StreamFromId *m_config_sfi_owned;
    Configuration::PathFromId *m_config_pfi_owned;
    Configuration::TextureFromStream *m_config_tfs_owned;

    // Managers
    Renderer *m_renderer;
    TextManager *m_text_manager;
    TextureManager *m_texture_manager;

    // Root
    Layout *m_root;

    // Input state
    Layout *m_over;

    // Lua
    class LuaStackChecker {
    public:
      LuaStackChecker(lua_State *L, Environment *env);
      ~LuaStackChecker();

    private:
      int m_depth;
      lua_State *m_L;
      Environment *m_env;
    };
    std::set<lua_State *> m_lua_environments;
  };
}

#endif
