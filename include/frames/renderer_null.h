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

