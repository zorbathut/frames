// The Environment class

#ifndef FRAMES_ENVIRONMENT
#define FRAMES_ENVIRONMENT

#include "frames/configuration.h"
#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/renderer.h"

#include <deque>

namespace Frames {
  class Environment : Noncopyable {
  public:
    Environment();  // init to default
    Environment(const Configuration &config);
    ~Environment();

    void ResizeRoot(int x, int y);
    
    void Render(LayoutPtr root = 0);
    
    const LayoutPtr &GetRoot() { return m_root; }
    const Configuration &GetConfiguration() { return m_config; }

    // Internal only, do not call
    void LogError(const std::string &log) { m_config.logger->LogError(log); }
    void LogDebug(const std::string &log) { m_config.logger->LogDebug(log); }
  private:
    friend class Layout;

    // Utility functions and parameters
    void Init(const Configuration &config);

    void MarkInvalidated(LayoutPtr layout);
    std::deque<LayoutPtr> m_invalidated;

    // Configuration
    Configuration m_config;
    Configuration::Logger *m_config_logger_owned;

    // Managers
    Renderer m_renderer;

    // Root
    LayoutPtr m_root;
  };
}

#endif
