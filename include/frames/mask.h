// The Mask class

#ifndef FRAMES_MASK
#define FRAMES_MASK

#include "frames/frame.h"

namespace Frames {
  class Mask : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    static Mask *Create(const std::string &name, Layout *parent);

  protected:
    virtual bool MouseMaskingTest(float x, float y);

  private:
    Mask(const std::string &name, Layout *parent);
    virtual ~Mask();

    virtual void RenderElementPreChild(detail::Renderer *renderer) const;
    virtual void RenderElementPostChild(detail::Renderer *renderer) const;
  };
}

#endif
