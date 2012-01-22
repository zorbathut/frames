// The Frame class

#ifndef FRAMES_FRAME
#define FRAMES_FRAME

#include "frames/layout.h"

namespace Frames {
  class Frame : public Layout {
  public:
    using Layout::SetPoint;

    using Layout::SetSize;
    using Layout::SetWidth;
    using Layout::SetHeight;

    using Layout::SetParent;
    using Layout::GetParent;

    using Layout::SetLayer;
    using Layout::GetLayer;

    using Layout::SetStrata;
    using Layout::GetStrata;

    using Layout::SetVisible;
    using Layout::GetVisible;

    void SetBackground(float r, float g, float b, float a = 1.0f);
    void GetBackground(float *r, float *g, float *b, float *a);

  private:
    virtual void RenderElement();

    Frame();  // We'll need more here
    ~Frame();
  };
}

#endif
