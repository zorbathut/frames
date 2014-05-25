// Renderer core

#ifndef FRAMES_RENDERER
#define FRAMES_RENDERER

#include <stack>
#include <vector>

#include "frames/color.h"
#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/rect.h"
#include "frames/texture.h"

namespace Frames {
  class Environment;
  struct Rect;
  class Texture;
  typedef Ptr<Texture> TexturePtr;

  namespace Configuration {
    class Renderer;
    typedef Ptr<Renderer> RendererPtr;
  }

  namespace detail {
    class TextureBacking;
    typedef Ptr<TextureBacking> TextureBackingPtr;
    class TextureChunk;
    typedef Ptr<TextureChunk> TextureChunkPtr;

    class TextureBacking : public Refcountable<TextureBacking> {
      friend class Refcountable<TextureBacking>;
    public:

      int WidthGet() const { return m_surface_width; }
      int HeightGet() const { return m_surface_height; }

      Environment *EnvironmentGet() const { return m_env; }

      virtual void Write(int sx, int sy, const TexturePtr &tex) = 0;

      std::pair<int, int> SubtextureAllocate(int width, int height);

    protected:
      TextureBacking(Environment *env, int width, int height);
      virtual ~TextureBacking();

    private:
      Environment *m_env;

      int m_surface_width;
      int m_surface_height;

      int m_alloc_next_x;
      int m_alloc_cur_y;
      int m_alloc_next_y;
    };

    class Renderer : Noncopyable {
    public:
      struct Vertex {
        Vector p;
        Vector t;
        Color c;
      };

      Renderer(Environment *env);
      ~Renderer();

      Environment *EnvironmentGet() const { return m_env;  }

      // should almost certainly be overloaded; must then have these called
      virtual void Begin(int width, int height);
      virtual void End();

      virtual Vertex *Request(int quads) = 0;
      virtual void Return(int quads = -1) = 0;  // also renders, count lets you optionally specify the number of quads

      virtual TextureBackingPtr TextureCreate(int width, int height, Texture::Format mode) = 0;
      virtual void TextureSet(const TextureBackingPtr &tex) = 0;

      void ScissorPush(Rect rect);
      void ScissorPop();

      virtual void StatePush() = 0;
      virtual void StateClean() = 0;
      virtual void StatePop() = 0;

      void AlphaPush(float alpha);
      float AlphaGet() const;
      void AlphaPop();
    
      static bool WriteCroppedRect(Vertex *vertex, const Rect &screen, const Color &color, const Rect &bounds); // no fancy lerping
      static bool WriteCroppedTexRect(Vertex *vertex, const Rect &screen, const Rect &tex, const Color &color, const Rect &bounds);  // fancy lerping

    protected:
      int WidthGet() { return m_width; }
      int HeightGet() { return m_height; }

    private:
      Environment *m_env; // just for debug functionality

      int m_width;
      int m_height;

      virtual void ScissorSet(const Rect &rect) = 0;
      std::stack<Rect> m_scissor;

      std::vector<float> m_alpha; // we'll only really allocate it once
    };
  }
}

#endif

