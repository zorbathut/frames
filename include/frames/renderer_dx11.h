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

#ifndef FRAMES_RENDERER_DX11
#define FRAMES_RENDERER_DX11

#include <stack>
#include <vector>

#include "frames/renderer.h"

#include "frames/configuration.h"

#include <d3d11.h>

namespace Frames {
  namespace Configuration {
    /// Creates a Configuration::Renderer for DX11.
    /** Frames will handle its own refcounting for this device context; you are welcome to ->Release() it immediately after calling this function. */
    RendererPtr RendererDX11(ID3D11DeviceContext *device);
  }
  
  namespace detail {
    class TextureBackingDX11 : public TextureBacking {
    public:
      TextureBackingDX11(Environment *env, int width, int height, Texture::Format format);
      ~TextureBackingDX11();

      virtual void Write(int sx, int sy, const TexturePtr &tex) FRAMES_OVERRIDE;

      ID3D11Texture2D *TexGet() const { return m_tex;  }
      ID3D11ShaderResourceView *ShaderResourceViewGet() const { return m_srview; }

    private:
      ID3D11Texture2D *m_tex;
      ID3D11ShaderResourceView *m_srview;
    };

    class RendererDX11 : public Renderer {
    public:
      RendererDX11(Environment *env, ID3D11DeviceContext *context);
      ~RendererDX11();

      virtual void Begin(int width, int height) FRAMES_OVERRIDE;
      virtual void End() FRAMES_OVERRIDE;

      virtual Vertex *Request(int quads) FRAMES_OVERRIDE;
      virtual void Return(int quads = -1) FRAMES_OVERRIDE;  // also renders, count lets you optionally specify the number of quads

      virtual TextureBackingPtr TextureCreate(int width, int height, Texture::Format mode) FRAMES_OVERRIDE;
      virtual void TextureSet(const TextureBackingPtr &tex) FRAMES_OVERRIDE;

      ID3D11DeviceContext *ContextGet() const { return m_context; }
      ID3D11Device *DeviceGet() const { return m_device;  }

    private:
      void CreateBuffers(int len);

      ID3D11Device *m_device;
      ID3D11DeviceContext *m_context;

      ID3D11RasterizerState *m_rasterizerState;
      ID3D11BlendState *m_blendState;
      ID3D11DepthStencilState *m_depthState;

      ID3D11VertexShader *m_vs;
      ID3D11PixelShader *m_ps;

      // shader indices
      int m_shader_ci_size;
      int m_shader_ci_item;
      int m_shader_tex;
      int m_shader_sample;

      ID3D11Buffer *m_shader_ci_size_buffer;

      // may as well keep 'em all around
      ID3D11Buffer *m_shader_ci_item_buffer_sample_off;
      ID3D11Buffer *m_shader_ci_item_buffer_sample_full;
      ID3D11Buffer *m_shader_ci_item_buffer_sample_alpha;

      ID3D11SamplerState *m_sampler;

      ID3D11InputLayout *m_verticesLayout;
      ID3D11Buffer *m_vertices;
      int m_verticesQuadcount;
      int m_verticesQuadpos;
    
      int m_verticesLastQuadpos;
      int m_verticesLastQuadsize;

      ID3D11Buffer *m_indices;

      ID3D11ShaderResourceView *m_currentTexture;

      virtual void ScissorSet(const Rect &rect) FRAMES_OVERRIDE;
    };
  }
}

#endif

