// The Frame class

#ifndef FRAMES_FRAME
#define FRAMES_FRAME

#include "frames/layout.h"

namespace Frames {
  class Frame : public Layout {

  private:
    virtual void RenderElement();

    Frame();  // We'll need more here
    ~Frame();
  };
}

#endif
