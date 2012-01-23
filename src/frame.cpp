
#include "frames/frame.h"

#include <GL/gl.h>

namespace Frames {
  FramePtr Frame::Create(LayoutPtr parent) {
    return new Frame(parent);
  }

  void Frame::SetBackground(float r, float g, float b, float a) {
    if (r != m_bg_r || g != m_bg_g || b != m_bg_b || a != m_bg_a) {
      m_bg_r = r;
      m_bg_g = g;
      m_bg_b = b;
      m_bg_a = a;
    }
  }

  void Frame::GetBackground(float *r, float *g, float *b, float *a) {
    *r = m_bg_r;
    *g = m_bg_g;
    *b = m_bg_b;
    *a = m_bg_a;
  }

  void Frame::RenderElement() {
    // zomg inefficient
    float u = GetTop();
    float d = GetBottom();
    float l = GetLeft();
    float r = GetRight();
    glColor4f(m_bg_r, m_bg_g, m_bg_b, m_bg_a);
    glBegin(GL_QUADS);
    glVertex2f(l, u);
    glVertex2f(r, u);
    glVertex2f(r, d);
    glVertex2f(l, d);
    glEnd();
  }

  Frame::Frame(const LayoutPtr &parent) :
      Layout(parent),
      m_bg_r(0),
      m_bg_g(0),
      m_bg_b(0),
      m_bg_a(0)
  { };
  Frame::~Frame() { };
}

