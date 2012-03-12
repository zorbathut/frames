// The Environment class

#ifndef FRAMES_ENVIRONMENT
#define FRAMES_ENVIRONMENT

#include "frames/configuration.h"
#include "frames/noncopyable.h"
#include "frames/utility.h"

#include <deque>
#include <vector>

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

    void ResizeRoot(int x, int y);
    
    void Render(const Layout *root = 0);
    
    Layout *GetRoot() { return m_root; }
    const Configuration &GetConfiguration() { return m_config; }

    void RegisterLua(lua_State *L);
    void RegisterLuaFramesBuiltin(lua_State *L);
    template<typename T> void RegisterLuaFrame(lua_State *L);

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

    // Lua debugging
    class LuaStackChecker {
    public:
      LuaStackChecker(lua_State *L, Environment *env);
      ~LuaStackChecker();

    private:
      int m_depth;
      lua_State *m_L;
      Environment *m_env;
    };
  };
}

#endif
