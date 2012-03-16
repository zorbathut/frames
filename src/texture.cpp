
#include "frames/texture.h"

#include "frames/environment.h"
#include "frames/lua.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include <GL/GLew.h>

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

  /*static*/ const char *Texture::GetStaticType() {
    return "Texture";
  }

  void Texture::SetTexture(const std::string &id) {
    // work work work
    m_texture_id = id;
    m_texture = GetEnvironment()->GetTextureManager()->TextureFromId(id);
    
    SetWidthDefault(m_texture->GetWidth());
    SetHeightDefault(m_texture->GetHeight());
  }

  /*static*/ void Texture::l_RegisterFunctions(lua_State *L) {
    Frame::l_RegisterFunctions(L);

    l_RegisterFunction(L, GetStaticType(), "SetTexture", l_SetTexture);
    l_RegisterFunction(L, GetStaticType(), "GetTexture", l_GetTexture);
  }

  void Texture::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

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

      v[0].u = m_texture->GetSX(); v[0].v = m_texture->GetSY();
      v[1].u = m_texture->GetEX(); v[1].v = m_texture->GetSY();
      v[2].u = m_texture->GetEX(); v[2].v = m_texture->GetEY();
      v[3].u = m_texture->GetSX(); v[3].v = m_texture->GetEY();

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

  /*static*/ int Texture::l_SetTexture(lua_State *L) {
    l_checkparams(L, 2);
    Texture *self = l_checkframe<Texture>(L, 1);

    self->SetTexture(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Texture::l_GetTexture(lua_State *L) {
    l_checkparams(L, 1);
    Texture *self = l_checkframe<Texture>(L, 1);

    lua_pushstring(L, self->GetTexture().c_str());

    return 1;
  }
}

