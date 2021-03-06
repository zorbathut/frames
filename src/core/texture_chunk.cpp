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

#include "frames/texture_chunk.h"

#include "frames/environment.h"
#include "frames/renderer.h"

namespace Frames {
  namespace detail {
    TextureChunk::TextureChunk() :
        m_texture_width(0),
        m_texture_height(0),
        m_bounds(0, 0, 0, 0)
    {
    }

    TextureChunk::~TextureChunk() {
      m_backing->EnvironmentGet()->TextureChunkShutdown(this);
    }

    /*static*/ TextureChunkPtr TextureChunk::Create() {
      return TextureChunkPtr(new TextureChunk());
    }

    void TextureChunk::Attach(const TextureBackingPtr &backing, int sx, int sy, int ex, int ey) {
      m_backing = backing;
      m_texture_width = ex - sx;
      m_texture_height = ey - sy;
      m_bounds.s.x = (float)sx / backing->WidthGet();
      m_bounds.s.y = (float)sy / backing->HeightGet();
      m_bounds.e.x = m_bounds.s.x + (float)m_texture_width / backing->WidthGet();
      m_bounds.e.y = m_bounds.s.y + (float)m_texture_height / backing->HeightGet();
    }
  }
}

