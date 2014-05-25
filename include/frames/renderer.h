// Renderer core

#ifndef FRAMES_RENDERER
#define FRAMES_RENDERER

#include <stack>
#include <vector>

#include "frames/color.h"
#include "frames/noncopyable.h"
#include "frames/rect.h"

#include "frames/texture_manager.h"

namespace Frames {
  class Environment;
  struct Rect;
  template <typename T> class Ptr;

  namespace detail {
    class TextureBacking;
    typedef Ptr<TextureBacking> TextureBackingPtr;
    class TextureChunk;
    typedef Ptr<TextureChunk> TextureChunkPtr;

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

      // Texture manipulation - refactoring in progress!
      TextureBackingPtr BackingCreate(int width, int height, int mode);

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

      // Texture manipulation - refactoring in progress!
      friend class TextureBacking;
      void Internal_Init_Backing(TextureBacking *backing);
      void Internal_Shutdown_Backing(TextureBacking *backing);
    };
  }
}

#endif

