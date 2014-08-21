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

#include "frames/renderer_null.h"

#include "frames/configuration.h"
#include "frames/detail.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
#include "frames/rect.h"

#include <vector>
#include <algorithm>

using namespace std;

namespace Frames {
  namespace Configuration {
    class CfgRendererNull : public Renderer {
    public:
      CfgRendererNull() { }

      virtual detail::Renderer *Create(Environment *env) const FRAMES_OVERRIDE {
        return new detail::RendererNull(env);
      }
    };

    Configuration::RendererPtr Configuration::RendererNull() {
      return Configuration::RendererPtr(new CfgRendererNull());
    }
  }

  namespace detail {
    TextureBackingNull::TextureBackingNull(Environment *env, int width, int height, Texture::Format format) : TextureBacking(env, width, height, format) { }
    TextureBackingNull::~TextureBackingNull() { }

    void TextureBackingNull::Write(int sx, int sy, const TexturePtr &tex) { }

    RendererNull::RendererNull(Environment *env) : Renderer(env) { }

    RendererNull::~RendererNull() { }

    void RendererNull::Begin(int width, int height) {
      Renderer::Begin(width, height);
    }

    void RendererNull::End() {
    }

    Renderer::Vertex *RendererNull::Request(int quads) {
      return 0; // this is valid! it's an error condition for any renderer but this one, but it's valid
    }

    void RendererNull::Return(int quads /*= -1*/) {
      EnvironmentGet()->LogError("Vertices returned to null renderer somehow"); // We never give out vertices, so we should never get vertices returned
    }

    TextureBackingPtr RendererNull::TextureCreate(int width, int height, Texture::Format mode) {
      return TextureBackingPtr(new TextureBackingNull(EnvironmentGet(), width, height, mode));
    }

    void RendererNull::TextureSet(const detail::TextureBackingPtr &tex) { }

    void RendererNull::ScissorSet(const Rect &rect) { }
  }
}

