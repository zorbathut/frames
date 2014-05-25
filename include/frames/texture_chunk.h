// Texture chunk

#ifndef FRAMES_TEXTURE_CHUNK
#define FRAMES_TEXTURE_CHUNK

#include "frames/ptr.h"
#include "frames/rect.h"

namespace Frames {
  namespace detail {
    class TextureBacking;
    typedef Ptr<TextureBacking> TextureBackingPtr;
    class TextureChunk;
    typedef Ptr<TextureChunk> TextureChunkPtr;

    class TextureChunk : public Refcountable<TextureChunk> {
      friend class Refcountable<TextureChunk>;
    public:
      static TextureChunkPtr Create();

      int WidthGet() const { return m_texture_width; }
      int HeightGet() const { return m_texture_height; }

      const Rect &BoundsGet() { return m_bounds; }

      const TextureBackingPtr &BackingGet() { return m_backing; }

      void Attach(const TextureBackingPtr &backing, int sx, int sy, int ex, int ey);

    private:
      TextureChunk();
      ~TextureChunk();

      friend class TextureManager;

      TextureBackingPtr m_backing;

      int m_texture_width;
      int m_texture_height;

      Rect m_bounds;
    };
  }
}

#endif
