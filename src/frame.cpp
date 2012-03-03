
#include "frames/frame.h"

#include "frames/environment.h"
#include "frames/renderer.h"

#include <GL/gl.h>

namespace Frames {
  Frame *Frame::CreateBare(Layout *parent) {
    return new Frame(parent);
  }
  Frame *Frame::CreateTagged_imp(const char *filename, int line, Layout *parent) {
    Frame *rv = new Frame(parent);
    rv->SetNameStatic(filename);
    rv->SetNameId(line);
    return rv;
  }

  void Frame::SetBackground(float r, float g, float b, float a) {
    if (r != m_bg_r || g != m_bg_g || b != m_bg_b || a != m_bg_a) {
      m_bg_r = r;
      m_bg_g = g;
      m_bg_b = b;
      m_bg_a = a;
    }
  }

  void Frame::GetBackground(float *r, float *g, float *b, float *a) const {
    *r = m_bg_r;
    *g = m_bg_g;
    *b = m_bg_b;
    *a = m_bg_a;
  }

  void Frame::RenderElement(Renderer *renderer) const {
    if (m_bg_a > 0) {
      renderer->SetTexture();

      float u = GetTop();
      float d = GetBottom();
      float l = GetLeft();
      float r = GetRight();

      Renderer::Vertex *v = renderer->Request(4);

      v[0].x = l; v[0].y = u;
      v[1].x = r; v[1].y = u;
      v[2].x = r; v[2].y = d;
      v[3].x = l; v[3].y = d;

      v[0].r = m_bg_r; v[0].g = m_bg_g; v[0].b = m_bg_b; v[0].a = m_bg_a;
      v[1].r = m_bg_r; v[1].g = m_bg_g; v[1].b = m_bg_b; v[1].a = m_bg_a;
      v[2].r = m_bg_r; v[2].g = m_bg_g; v[2].b = m_bg_b; v[2].a = m_bg_a;
      v[3].r = m_bg_r; v[3].g = m_bg_g; v[3].b = m_bg_b; v[3].a = m_bg_a;

      renderer->Return(GL_QUADS);
    }
  }

  Frame::Frame(Layout *parent) :
      Layout(parent),
      m_bg_r(0),
      m_bg_g(0),
      m_bg_b(0),
      m_bg_a(0)
  { };
  Frame::~Frame() { };
}

