// Renderer core

#ifndef FRAMES_RENDERER_UE4_RHI
#define FRAMES_RENDERER_UE4_RHI

// must be first; ue4 is picky
#include "RHI.h"

#include <stack>
#include <vector>

#include "frames/renderer.h"

#include "frames/configuration.h"

namespace Frames {
  namespace Configuration {
    /// Creates a Configuration::Renderer for UE4 RHI.
    RendererPtr RendererUE4RHI();
  }
  
  namespace detail {
    class TextureBackingUE4RHI : public TextureBacking {
    public:
      TextureBackingUE4RHI(Environment *env, int width, int height, Texture::Format format);
      ~TextureBackingUE4RHI();

      virtual void Write(int sx, int sy, const TexturePtr &tex);

      //ID3D11Texture2D *TexGet() const { return m_tex;  }
      //ID3D11ShaderResourceView *ShaderResourceViewGet() const { return m_srview; }

    private:
      FHRITexture2D *m_tex;
      //ID3D11Texture2D *m_tex;
      //ID3D11ShaderResourceView *m_srview;
    };

    class RendererUE4RHI : public Renderer {
    public:
      RendererUE4RHI(Environment *env, ID3D11DeviceContext *context);
      ~RendererUE4RHI();

      virtual void Begin(int width, int height);
      virtual void End();

      virtual Vertex *Request(int quads);
      virtual void Return(int quads = -1);  // also renders, count lets you optionally specify the number of quads

      virtual TextureBackingPtr TextureCreate(int width, int height, Texture::Format mode);
      virtual void TextureSet(const TextureBackingPtr &tex);

      //ID3D11DeviceContext *ContextGet() const { return m_context; }
      //ID3D11Device *DeviceGet() const { return m_device;  }

    private:
    
      /*void CreateBuffers(int len);

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

      // may as well keep 'em both around
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

      ID3D11ShaderResourceView *m_currentTexture;*/

      virtual void ScissorSet(const Rect &rect);
    };
  }
}

#endif

