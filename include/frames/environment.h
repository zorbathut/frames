// The Environment class

#ifndef FRAMES_ENVIRONMENT
#define FRAMES_ENVIRONMENT

#include "frames/noncopyable.h"
#include "frames/ptr.h"

class Configuration;

namespace Frames {
  class Environment : Noncopyable {
    Environment();  // init to default
    Environment(Configuration *config);
    ~Environment();
    
    void Render();
    
    LayoutPtr GetRoot();
    
    FramePtr CreateFrame(LayoutPtr parent);
  };
}

#endif
