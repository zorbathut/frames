// The Mask class

#ifndef FRAMES_MASK
#define FRAMES_MASK

#include "frames/frame.h"

namespace Frames {
  class Mask : public Frame {
  public:
    static Mask *CreateBare(Layout *parent);
    static Mask *CreateTagged_imp(const char *filename, int line, Layout *parent);

  private:
    Mask(Layout *parent);
    virtual ~Mask();

    virtual void RenderElement(Renderer *renderer) const;
    virtual void RenderElementPost(Renderer *renderer) const;
  };
}

#endif
