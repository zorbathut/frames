
#include "frames/frame.h"

#include "frames/cast.h"
#include "frames/environment.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include <cmath>

namespace Frames {
  FRAMES_DEFINE_RTTI(Frame, Layout);

  Frame *Frame::Create(Layout *parent, const std::string &name = "") {
    if (!parent) {
      Configuration::Get().LoggerGet()->LogError("Attempted to create Frame with null parent");
      return 0;
    }
    return new Frame(parent, name);
  }

  void Frame::BackgroundSet(const Color &color) {
    if (color != m_bg) {
      m_bg = color;
    }
  }

  void Frame::RenderElement(detail::Renderer *renderer) const {
    if (m_bg.a > 0) {
      Color bgc = m_bg * Color(1, 1, 1, renderer->AlphaGet());

      renderer->TextureSet(detail::TextureBackingPtr());

      // clamp to int to avoid rounding errors
      float u = std::floor(TopGet() + 0.5f);
      float d = std::floor(BottomGet() + 0.5f);
      float l = std::floor(LeftGet() + 0.5f);
      float r = std::floor(RightGet() + 0.5f);

      detail::Renderer::Vertex *v = renderer->Request(1);

      v[0].p.x = l; v[0].p.y = u;
      v[1].p.x = r; v[1].p.y = u;
      v[2].p.x = r; v[2].p.y = d;
      v[3].p.x = l; v[3].p.y = d;

      v[0].c = bgc;
      v[1].c = bgc;
      v[2].c = bgc;
      v[3].c = bgc;

      renderer->Return();
    }
  }

  Frame::Frame(Layout *parent, const std::string &name) :
    Layout(parent->EnvironmentGet(), name),
      m_bg(0, 0, 0, 0)
  {
    ParentSet(parent);
  }
  Frame::~Frame() { }
}