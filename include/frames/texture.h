// The Texture class

#ifndef FRAME_TEXTURE
#define FRAME_TEXTURE

#include "frames/frame.h"
#include "frames/texture_manager.h"

namespace Frame {
  class Texture : public Frame {
    friend class Environment;

  public:
    static Texture *CreateBare(Layout *parent);
    static Texture *CreateTagged_imp(const char *filename, int line, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }

    void SetTexture(const std::string &id);
    const std::string &GetTexture() const { return m_texture_id; }

    // EXPERIMENTAL
    void SetTint(Color color);
    Color GetTint() const { return m_tint; }

  protected:
    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, GetStaticType()); Frame::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Texture(Layout *parent);
    virtual ~Texture();

    virtual void RenderElement(Renderer *renderer) const;

    std::string m_texture_id;
    TextureChunkPtr m_texture;

    Color m_tint;

    // Lua bindings
    static int luaF_SetTexture(lua_State *L);
    static int luaF_GetTexture(lua_State *L);

    static int luaF_SetTint(lua_State *L);
    static int luaF_GetTint(lua_State *L);
  };
}

#endif
