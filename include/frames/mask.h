// The Mask class

#ifndef FRAMES_MASK
#define FRAMES_MASK

#include "frames/frame.h"

namespace Frames {
  /// Used for clipping frames.
  /** All children of Mask will be clipped to the dimensions of Mask.
  This is useful for situations where part of a frame may be visible, but the invisible part is not occluded by a frame "above" it.
  A good example is a scrollable list - the body of the list would be contained in a Mask frame.
  
  Mask will also clip mouse input, ensuring that the user can't accidentally click on a clipped UI element.
  
  Mask has no extra public API - its functionality is a property of the type itself.*/
  class Mask : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    /// Creates a new Mask.
    static Mask *Create(Layout *parent, const std::string &name);

  private:
    Mask(Layout *parent, const std::string &name);
    virtual ~Mask();

    virtual bool MouseMaskingTest(float x, float y);

    virtual void RenderElementPreChild(detail::Renderer *renderer) const;
    virtual void RenderElementPostChild(detail::Renderer *renderer) const;
  };
}

#endif
