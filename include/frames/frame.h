// The Frame class

#ifndef FRAMES_FRAME
#define FRAMES_FRAME

#include "frames/layout.h"

namespace Frames {
  class Frame : public Layout {
  public:
    static Frame *CreateBare(Layout *parent);
    static Frame *CreateTagged_imp(const char *filename, int line, Layout *parent);

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

    using Layout::Obliterate;

  protected:
    Frame(Layout *parent);
    virtual ~Frame();

  private:
    virtual void RenderElement(Renderer *renderer) const;

    float m_bg_r, m_bg_g, m_bg_b, m_bg_a;
  };
}

#endif
