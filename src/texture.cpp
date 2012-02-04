
#include "frames/texture.h"

#include "frames/environment.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include <GL/gl.h>

namespace Frames {
  Texture *Texture::CreateBare(Layout *parent) {
    return new Texture(parent);
  }
  Texture *Texture::CreateTagged_imp(const char *filename, int line, Layout *parent) {
    Texture *rv = new Texture(parent);
    rv->SetNameStatic(filename);
    rv->SetNameId(line);
    return rv;
  }

  void Texture::SetTexture(const std::string &id) {
    // work work work
    m_texture = GetEnvironment()->GetTextureManager()->TextureFromId(id);
    
    SetWidthDefault(m_texture->texture_width);
    SetHeightDefault(m_texture->texture_height);
  }

  void Texture::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

    if (m_texture) {
      renderer->SetTexture(m_texture->id);

      float u = GetTop();
      float d = GetBottom();
      float l = GetLeft();
      float r = GetRight();

      Renderer::Vertex *v = renderer->Request(4);

      v[0].x = l; v[0].y = u;
      v[1].x = r; v[1].y = u;
      v[2].x = r; v[2].y = d;
      v[3].x = l; v[3].y = d;

      v[0].t = m_texture->sx; v[0].u = m_texture->sy;
      v[1].t = m_texture->ex; v[1].u = m_texture->sy;
      v[2].t = m_texture->ex; v[2].u = m_texture->ey;
      v[3].t = m_texture->sx; v[3].u = m_texture->ey;

      v[0].r = 1; v[0].g = 1; v[0].b = 1; v[0].a = 1;
      v[1].r = 1; v[1].g = 1; v[1].b = 1; v[1].a = 1;
      v[2].r = 1; v[2].g = 1; v[2].b = 1; v[2].a = 1;
      v[3].r = 1; v[3].g = 1; v[3].b = 1; v[3].a = 1;

      renderer->Return(GL_QUADS);
    }
  }

  Texture::Texture(Layout *parent) :
      Frame(parent)
  { };
  Texture::~Texture() { };
}

