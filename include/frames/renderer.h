/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

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
      Texture::Format FormatGet() const { return m_surface_format;  }

      Environment *EnvironmentGet() const { return m_env; }

      virtual void Write(int sx, int sy, const TexturePtr &tex) = 0;

      std::pair<int, int> SubtextureAllocate(int width, int height);

    protected:
      TextureBacking(Environment *env, int width, int height, Texture::Format format);
      virtual ~TextureBacking();

    private:
      Environment *m_env;

      int m_surface_width;
      int m_surface_height;
      Texture::Format m_surface_format;

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
      virtual ~Renderer();

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

