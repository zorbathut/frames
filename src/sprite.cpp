
#include "frames/sprite.h"

#include "frames/cast.h"
#include "frames/environment.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include "frames/os_gl.h"

namespace Frames {
  FRAMES_DEFINE_RTTI(Sprite, Frame);

  Sprite *Sprite::Create(const std::string &name, Layout *parent) {
    return new Sprite(name, parent);
  }

  void Sprite::TextureSet(const std::string &id) {
    // work work work
    m_texture_id = id;
    m_texture = EnvironmentGet()->GetTextureManager()->TextureFromId(id);
    
    WidthDefaultSet((float)m_texture->WidthGet());
    HeightDefaultSet((float)m_texture->HeightGet());
  }

  void Sprite::RenderElement(detail::Renderer *renderer) const {
    Frame::RenderElement(renderer);

    if (m_texture) {
      Color tint = m_tint * Color(1, 1, 1, renderer->AlphaGet());

      renderer->TextureSet(m_texture.Get());

      float u = TopGet();
      float d = BottomGet();
      float l = LeftGet();
      float r = RightGet();

      detail::Renderer::Vertex *v = renderer->Request(4);

      v[0].p.x = l; v[0].p.y = u;
      v[1].p.x = r; v[1].p.y = u;
      v[2].p.x = r; v[2].p.y = d;
      v[3].p.x = l; v[3].p.y = d;

      v[0].t = m_texture->BoundsGet().s;
      v[2].t = m_texture->BoundsGet().e;
      
      v[1].t.x = v[2].t.x;
      v[1].t.y = v[0].t.y;
      
      v[3].t.x = v[0].t.x;
      v[3].t.y = v[2].t.y;

      v[0].c = tint;
      v[1].c = tint;
      v[2].c = tint;
      v[3].c = tint;

      renderer->Return(GL_QUADS);
    }
  }

  Sprite::Sprite(const std::string &name, Layout *parent) :
      Frame(name, parent),
      m_tint(1, 1, 1, 1)
  { };
  Sprite::~Sprite() { };
}

