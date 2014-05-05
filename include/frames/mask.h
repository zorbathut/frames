// The Mask class

#ifndef FRAMES_MASK
#define FRAMES_MASK

#include "frames/frame.h"

namespace Frames {
  class Mask : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    static Mask *Create(Layout *parent, const std::string &name);

  protected:
    virtual bool MouseMaskingTest(float x, float y);

  private:
    Mask(Layout *parent, const std::string &name);
    virtual ~Mask();

    virtual void RenderElementPreChild(detail::Renderer *renderer) const;
    virtual void RenderElementPostChild(detail::Renderer *renderer) const;
  };
}

#endif
