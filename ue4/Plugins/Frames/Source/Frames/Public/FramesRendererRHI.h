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

#ifndef FRAMES_RENDERER_RHI
#define FRAMES_RENDERER_RHI

// for whatever reason, ue4.3 broke several includes, so now we need to include this even though we don't really use it
// thanks obama
#include "Engine.h"

#include "RHI.h"
#include "RHIResources.h"

#include "frames/renderer.h"

#include "frames/configuration.h"

class FRHICommandList;

namespace Frames {
  namespace Configuration {
    /// Creates a Configuration::Renderer for UE4 RHI.
    RendererPtr RendererRHI();
  }
  
  namespace detail {

    // Actual data is stored in "Data" subclasses; this class's lifetime is dictated more by the render queue system than anything else
    // Needed because we pass data to the renderer, and the parent class may theoretically have been torn down by then
    // We manually handle this object's lifetime, which is a pain, but so it goes.

    class TextureBackingRHI : public TextureBacking {
    public:
      TextureBackingRHI(Environment *env, int width, int height, Texture::Format format);
      ~TextureBackingRHI();

      virtual void Write(int sx, int sy, const TexturePtr &tex);

      struct Data : detail::Noncopyable {
        FTexture2DRHIRef m_tex;
      };
      Data *DataGet() const { return m_rhi; }

    private:
      Data *m_rhi;
    };

    class RendererRHI : public Renderer {
    public:
      RendererRHI(Environment *env);
      ~RendererRHI();

      virtual void Begin(int width, int height);
      virtual void End();

      virtual Vertex *Request(int quads);
      virtual void Return(int quads = -1);  // also renders, count lets you optionally specify the number of quads

      virtual TextureBackingPtr TextureCreate(int width, int height, Texture::Format mode);
      virtual void TextureSet(const TextureBackingPtr &tex);

    private:
      void CreateBuffers(int len);

      struct Data : detail::Noncopyable {
        FVertexDeclarationRHIRef m_vertexDecl;

        FVertexBufferRHIRef m_vertices;

        FIndexBufferRHIRef m_indices;
      };

      Data *m_rhi;

      struct RequestData : detail::Noncopyable {
        RequestData();
        ~RequestData();

        int quads;
        Renderer::Vertex *data;
      };

      RequestData *m_request;

      int m_verticesQuadcount;

      TextureBackingRHI *m_currentTexture;

      virtual void ScissorSet(const Rect &rect);
    };
  }
}

#endif

