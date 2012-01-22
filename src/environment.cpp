
#include "frames/environment.h"
#include "frames/frame.h"

namespace Frames {
  Environment::Environment() {
    Configuration config;
    Init(&config);
  }
  Environment::Environment(const Configuration *config) {
    Init(config);
  }
  Environment::~Environment() { };

  void Environment::Render() {
    // While root isn't resolved
      // Resolve everything
      // Flush events
    // GC step?
    m_root->Render();
  };
  
  const LayoutPtr &Environment::GetRoot() {
    return m_root;
  }
  
  FramePtr CreateFrame(LayoutPtr parent) {
    return FramePtr();
  }

  void Environment::Init(const Configuration *config) {
    m_config = *config;
  }
}

