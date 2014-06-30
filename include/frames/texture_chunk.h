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

      TextureBackingPtr m_backing;

      int m_texture_width;
      int m_texture_height;

      Rect m_bounds;
    };
  }
}

#endif
