
#include "frames/environment.h"
#include "frames/frame.h"

namespace Frames {
  Environment::Environment() {
    Configuration config;
    Init(config);
  }
  Environment::Environment(const Configuration &config) {
    Init(config);
  }
  Environment::~Environment() {
    delete m_config_logger_owned;
  };

  void Environment::ResizeRoot(int x, int y) {
    m_root->SetWidth(x);
    m_root->SetHeight(y);
  }

  void Environment::Render() {
    // We want to batch up events if possible (todo: is this the case? which is faster - flushing as they go, or flushing all at once?) so, two nested loops
    while (!m_invalidated.empty()) {
      while (!m_invalidated.empty()) {
        LayoutPtr layout = m_invalidated.front();
        m_invalidated.pop_front();

        layout->Resolve();
      }

      // send events here
    }

    // GC step?

    m_root->Render();
  };

  void Environment::Init(const Configuration &config) {
    m_config_logger_owned = 0;

    m_config = config;

    if (!m_config.logger) {
      m_config_logger_owned = new Configuration::Logger();
      m_config.logger = m_config_logger_owned;
    }

    m_root = new Layout(0, this);
  }

  void Environment::MarkInvalidated(LayoutPtr layout) {
    m_invalidated.push_back(layout);
  }
}

