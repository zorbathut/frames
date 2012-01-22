// The Environment class

#ifndef FRAMES_ENVIRONMENT
#define FRAMES_ENVIRONMENT

#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/configuration.h"

namespace Frames {
  class Environment : Noncopyable {
  public:
    Environment();  // init to default
    Environment(const Configuration *config);
    ~Environment();
    
    void Render();
    
    const LayoutPtr &GetRoot();
    
    FramePtr CreateFrame(LayoutPtr parent);

  private:
    void Init(const Configuration *config);

    Configuration m_config;

    LayoutPtr m_root;
  };
}

#endif
