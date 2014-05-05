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
    static Sprite *Create(Layout *parent, const std::string &name);

    void TextureSet(const std::string &id);
    const std::string &TextureGet() const { return m_texture_id; }

    void EXPERIMENTAL_TintSet(Color color);
    Color EXPERIMENTAL_TintGet() const { return m_tint; }

  private:
    Sprite(Layout *parent, const std::string &name);
    virtual ~Sprite();

    virtual void RenderElement(detail::Renderer *renderer) const;

    std::string m_texture_id;
    detail::TextureChunkPtr m_texture;

    Color m_tint;
  };
}

#endif
