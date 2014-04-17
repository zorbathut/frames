
#include "frames/texture.h"

#include "frames/cast.h"
#include "frames/environment.h"
#include "frames/lua.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include "frames/os_gl.h"

namespace Frames {
  FRAMES_DEFINE_RTTI(Texture, Frame);

  Texture *Texture::Create(const std::string &name, Layout *parent) {
    return new Texture(name, parent);
  }

  /*static*/ const char *Texture::GetStaticType() {
    return "Texture";
  }

  void Texture::SetTexture(const std::string &id) {
    // work work work
    m_texture_id = id;
    m_texture = GetEnvironment()->GetTextureManager()->TextureFromId(id);
    
    SetWidthDefault((float)m_texture->GetWidth());
    SetHeightDefault((float)m_texture->GetHeight());
  }

  /*static*/ void Texture::luaF_RegisterFunctions(lua_State *L) {
    Frame::luaF_RegisterFunctions(L);

    luaF_RegisterFunction(L, GetStaticType(), "SetTexture", luaF_SetTexture);
    luaF_RegisterFunction(L, GetStaticType(), "GetTexture", luaF_GetTexture);

    luaF_RegisterFunction(L, GetStaticType(), "SetTint", luaF_SetTint);
    luaF_RegisterFunction(L, GetStaticType(), "GetTint", luaF_GetTint);
  }

  void Texture::RenderElement(detail::Renderer *renderer) const {
    Frame::RenderElement(renderer);

    if (m_texture) {
      Color tint = m_tint * Color(1, 1, 1, renderer->AlphaGet());

      renderer->SetTexture(m_texture.get());

      float u = GetTop();
      float d = GetBottom();
      float l = GetLeft();
      float r = GetRight();

      detail::Renderer::Vertex *v = renderer->Request(4);

      v[0].p.x = l; v[0].p.y = u;
      v[1].p.x = r; v[1].p.y = u;
      v[2].p.x = r; v[2].p.y = d;
      v[3].p.x = l; v[3].p.y = d;

      v[0].t = m_texture->GetBounds().s;
      v[2].t = m_texture->GetBounds().e;
      
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

  Texture::Texture(const std::string &name, Layout *parent) :
      Frame(name, parent),
      m_tint(1, 1, 1, 1)
  { };
  Texture::~Texture() { };

  /*static*/ int Texture::luaF_SetTexture(lua_State *L) {
    luaF_checkparams(L, 2);
    Texture *self = luaF_checkframe<Texture>(L, 1);

    self->SetTexture(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Texture::luaF_GetTexture(lua_State *L) {
    luaF_checkparams(L, 1);
    Texture *self = luaF_checkframe<Texture>(L, 1);

    lua_pushstring(L, self->GetTexture().c_str());

    return 1;
  }

  /*static*/ int Texture::luaF_SetTint(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Texture *self = luaF_checkframe<Texture>(L, 1);

    Color color(1, 1, 1, 1);
    color.r = (float)luaL_checknumber(L, 2);
    color.g = (float)luaL_checknumber(L, 3);
    color.b = (float)luaL_checknumber(L, 4);
    if (lua_gettop(L) == 5) {
      color.a = (float)luaL_checknumber(L, 5);
    }

    self->m_tint = color;

    return 0;
  }

  /*static*/ int Texture::luaF_GetTint(lua_State *L) {
    luaF_checkparams(L, 1);
    Texture *self = luaF_checkframe<Texture>(L, 1);

    lua_pushnumber(L, self->m_tint.r);
    lua_pushnumber(L, self->m_tint.g);
    lua_pushnumber(L, self->m_tint.b);
    lua_pushnumber(L, self->m_tint.a);

    return 4;
  }
}

