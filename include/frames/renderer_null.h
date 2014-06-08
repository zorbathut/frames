// Renderer core

#ifndef FRAMES_RENDERER_NULL
#define FRAMES_RENDERER_NULL

#include "frames/renderer.h"

#include "frames/configuration.h"

namespace Frames {
  namespace Configuration {
    /// Creates a Configuration::Renderer for Null.
    RendererPtr RendererNull();
  }
  
  namespace detail {
    class TextureBackingNull : public TextureBacking {
    public:
      TextureBackingNull(Environment *env, int width, int height, Texture::Format format);
      ~TextureBackingNull();

      virtual void Write(int sx, int sy, const TexturePtr &tex);
    };

    class RendererNull : public Renderer {
    public:
      RendererNull(Environment *env);
      ~RendererNull();

      virtual void Begin(int width, int height);
      virtual void End();

      virtual Vertex *Request(int quads);
      virtual void Return(int quads = -1);  // also renders, count lets you optionally specify the number of quads

      virtual TextureBackingPtr TextureCreate(int width, int height, Texture::Format mode);
      virtual void TextureSet(const TextureBackingPtr &tex);

    private:

      virtual void ScissorSet(const Rect &rect);
    };
  }
}

#endif

