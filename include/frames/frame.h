// The Frame class

#ifndef FRAMES_FRAME
#define FRAMES_FRAME

#include "frames/layout.h"

namespace Frames {
  class Frame : public Layout {
  public:
    static FramePtr CreateBare(LayoutPtr parent);
    static FramePtr CreateTagged_imp(const char *filename, int line, LayoutPtr parent);

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

  protected:
    Frame(const LayoutPtr &parent);
    virtual ~Frame();

  private:
    virtual void RenderElement(Renderer *renderer);

    float m_bg_r, m_bg_g, m_bg_b, m_bg_a;
  };
}

#endif
