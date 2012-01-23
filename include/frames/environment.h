// The Environment class

#ifndef FRAMES_ENVIRONMENT
#define FRAMES_ENVIRONMENT

#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/configuration.h"

#include <deque>

namespace Frames {
  class Environment : Noncopyable {
  public:
    Environment();  // init to default
    Environment(const Configuration &config);
    ~Environment();

    void ResizeRoot(int x, int y);
    
    void Render();
    
    const LayoutPtr &GetRoot() { return m_root; }
    const Configuration &GetConfiguration() { return m_config; }

  private:
    friend class Layout;

    void Init(const Configuration &config);

    void MarkInvalidated(LayoutPtr layout);
    std::deque<LayoutPtr> m_invalidated;

    Configuration m_config;
    Configuration::Logger *m_config_logger_owned;

    LayoutPtr m_root;
  };
}

#endif
