// The Sprite class

#ifndef FRAMES_TEXTURE
#define FRAMES_TEXTURE

#include "frames/frame.h"
#include "frames/texture_manager.h"

namespace Frames {
  /// Used to render a texture.
  /** The texture will be resized to fill the Sprite, possibly distorting its aspect ratio. */
  class Sprite : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    /// Creates a new Raw.
    static Sprite *Create(Layout *parent, const std::string &name);

    /// Sets the texture by ID.
    /** See \ref resources "Resources" for details on Frames's resource system.
    
    The Sprite's default size will be set to the native size of the texture, in pixels.*/
    void TextureSet(const std::string &id);
    /// Gets the current texture ID.
    const std::string &TextureGet() const { return m_texture_id; }

    // Experimental, disabled for documentation
    /// @cond EXPERIMENTAL
    void EXPERIMENTAL_TintSet(Color color);
    Color EXPERIMENTAL_TintGet() const { return m_tint; }
    /// @endcond

  protected:
    /// Creates a new Sprite with the given parameters. "parent" must be non-null.
    Sprite(Layout *parent, const std::string &name);
    virtual ~Sprite();

    /// Renders the Text.
    virtual void RenderElement(detail::Renderer *renderer) const;

  private:
    std::string m_texture_id;
    detail::TextureChunkPtr m_texture;

    Color m_tint;
  };
}

#endif
