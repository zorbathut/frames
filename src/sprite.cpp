
#include "frames/sprite.h"

#include "frames/cast.h"
#include "frames/environment.h"
#include "frames/lua.h"
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

  /*static*/ void Sprite::luaF_RegisterFunctions(lua_State *L) {
    Frame::luaF_RegisterFunctions(L);

    luaF_RegisterFunction(L, TypeStaticGet(), "TextureSet", luaF_SetTexture);
    luaF_RegisterFunction(L, TypeStaticGet(), "TextureGet", luaF_GetTexture);

    luaF_RegisterFunction(L, TypeStaticGet(), "TintSet", luaF_SetTint);
    luaF_RegisterFunction(L, TypeStaticGet(), "TintGet", luaF_GetTint);
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

  /*static*/ int Sprite::luaF_SetTexture(lua_State *L) {
    luaF_checkparams(L, 2);
    Sprite *self = luaF_checkframe<Sprite>(L, 1);

    self->TextureSet(luaL_checkstring(L, 2));

    return 0;
  }

  /*static*/ int Sprite::luaF_GetTexture(lua_State *L) {
    luaF_checkparams(L, 1);
    Sprite *self = luaF_checkframe<Sprite>(L, 1);

    lua_pushstring(L, self->TextureGet().c_str());

    return 1;
  }

  /*static*/ int Sprite::luaF_SetTint(lua_State *L) {
    luaF_checkparams(L, 4, 5);
    Sprite *self = luaF_checkframe<Sprite>(L, 1);

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

  /*static*/ int Sprite::luaF_GetTint(lua_State *L) {
    luaF_checkparams(L, 1);
    Sprite *self = luaF_checkframe<Sprite>(L, 1);

    lua_pushnumber(L, self->m_tint.r);
    lua_pushnumber(L, self->m_tint.g);
    lua_pushnumber(L, self->m_tint.b);
    lua_pushnumber(L, self->m_tint.a);

    return 4;
  }
}

