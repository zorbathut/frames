// The Sprite class

#ifndef FRAMES_TEXTURE
#define FRAMES_TEXTURE

#include "frames/frame.h"
#include "frames/texture_manager.h"

namespace Frames {
  class Sprite : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    static Sprite *Create(const std::string &name, Layout *parent);

    void TextureSet(const std::string &id);
    const std::string &TextureGet() const { return m_texture_id; }

    // EXPERIMENTAL
    void TintSet(Color color);
    Color TintGet() const { return m_tint; }

  protected:
    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, TypeStaticGet()); Frame::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Sprite(const std::string &name, Layout *parent);
    virtual ~Sprite();

    virtual void RenderElement(detail::Renderer *renderer) const;

    std::string m_texture_id;
    detail::TextureChunkPtr m_texture;

    Color m_tint;

    // Lua bindings
    static int luaF_SetTexture(lua_State *L);
    static int luaF_GetTexture(lua_State *L);

    static int luaF_SetTint(lua_State *L);
    static int luaF_GetTint(lua_State *L);
  };
}

#endif
