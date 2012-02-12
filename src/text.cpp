
#include "frames/text.h"

#include "frames/environment.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include <GL/GLew.h>

namespace Frames {
  Text *Text::CreateBare(Layout *parent) {
    return new Text(parent);
  }
  Text *Text::CreateTagged_imp(const char *filename, int line, Layout *parent) {
    Text *rv = new Text(parent);
    rv->SetNameStatic(filename);
    rv->SetNameId(line);
    return rv;
  }

  void Text::SetText(const std::string &text) {
    if (m_text != text) {
      m_text = text;
      
      // cache new glyphs
      UpdateDefaultSize();
    }
  }

  void Text::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // we'll fix this up further later
    if (m_texture) {
      renderer->SetTexture(m_texture.get());

      float u = GetTop();
      float d = GetBottom();
      float l = GetLeft();
      float r = GetRight();

      Renderer::Vertex *v = renderer->Request(4);

      v[0].x = l; v[0].y = u;
      v[1].x = r; v[1].y = u;
      v[2].x = r; v[2].y = d;
      v[3].x = l; v[3].y = d;

      v[0].t = m_texture->GetSX(); v[0].u = m_texture->GetSY();
      v[1].t = m_texture->GetEX(); v[1].u = m_texture->GetSY();
      v[2].t = m_texture->GetEX(); v[2].u = m_texture->GetEY();
      v[3].t = m_texture->GetSX(); v[3].u = m_texture->GetEY();

      v[0].r = 1; v[0].g = 1; v[0].b = 1; v[0].a = 1;
      v[1].r = 1; v[1].g = 1; v[1].b = 1; v[1].a = 1;
      v[2].r = 1; v[2].g = 1; v[2].b = 1; v[2].a = 1;
      v[3].r = 1; v[3].g = 1; v[3].b = 1; v[3].a = 1;

      renderer->Return(GL_QUADS);
    }
  }

  Text::Text(Layout *parent) :
      Frame(parent)
  {
    UpdateDefaultSize();
  };
  Text::~Text() { };
  
  void Text::UpdateDefaultSize() {
    // actually calculate
    SetWidthDefault(10 * m_text.size());
    SetHeightDefault(20);
  }
}

