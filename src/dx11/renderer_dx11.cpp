
#include "frames/renderer_dx11.h"

#include "frames/configuration.h"
#include "frames/detail.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
#include "frames/rect.h"

#include <vector>
#include <algorithm>

#include <d3d11.h>
#include <d3dcompiler.h>

using namespace std;

namespace Frames {
  namespace Configuration {
    class CfgRendererDX11 : public Renderer {
    public:
      CfgRendererDX11(ID3D11DeviceContext *device) : m_context(device) {}

      virtual detail::Renderer *Create(Environment *env) const {
        return new detail::RendererDX11(env, m_context);
      }

    private:
      ID3D11DeviceContext *m_context;
    };

    Configuration::RendererPtr Configuration::RendererDX11(ID3D11DeviceContext *context) {
      return Configuration::RendererPtr(new CfgRendererDX11(context));
    }
  }

  namespace detail {
    //The input-layout description
    static const D3D11_INPUT_ELEMENT_DESC sLayout[] =
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Renderer::Vertex, p), D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD0", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Renderer::Vertex, t), D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Renderer::Vertex, c), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    static const char sVertexShader[] = "float4 VS(float4 inPos : POSITION) : SV_POSITION { return inPos; }"; // I seem to be wearing a buffalo
    static const char sPixelShader[] = "float4 PS() : SV_TARGET { return float4(0.0f, 0.0f, 1.0f, 1.0f); }";

    TextureBackingDX11::TextureBackingDX11(Environment *env, int width, int height, Texture::Format format) : TextureBacking(env, width, height, format), m_tex(0) {
      D3D11_TEXTURE2D_DESC desc;
      memset(&desc, 0, sizeof(desc));
      desc.Width = width;
      desc.Height = height;
      desc.MipLevels = 1;
      desc.ArraySize = 1;
      if (format == Texture::FORMAT_RGBA_8) {
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      } else if (format == Texture::FORMAT_RGB_8) {
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      } else if (format == Texture::FORMAT_L_8) {
        desc.Format = DXGI_FORMAT_R8_UNORM;
      } else if (format == Texture::FORMAT_A_8) {
        desc.Format = DXGI_FORMAT_A8_UNORM;
      } else {
        EnvironmentGet()->LogError(detail::Format("Unrecognized raw type %d in texture", format));
        return;
      }
      desc.SampleDesc.Count = 1;
      desc.SampleDesc.Quality = 0;
      desc.Usage = D3D11_USAGE_DEFAULT; // someday we should make this immutable when possible
      desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      desc.MiscFlags = 0;

      if (static_cast<RendererDX11*>(env->RendererGet())->DeviceGet()->CreateTexture2D(&desc, 0, &m_tex) != S_OK) {
        env->LogError("DX11: Failure to create texture");
        return;
      }
    }

    TextureBackingDX11::~TextureBackingDX11() {
      m_tex->Release();
    }

    void TextureBackingDX11::Write(int sx, int sy, const TexturePtr &tex) {
      if (tex->TypeGet() != Texture::RAW) {
        EnvironmentGet()->LogError(detail::Format("Unrecognized type %d in texture", tex->TypeGet()));
        return;
      }

      if (tex->FormatGet() != FormatGet()) {
        EnvironmentGet()->LogError(detail::Format("Mismatched texture formats in TextureBacking::Write. Attempted write %d to %d", tex->FormatGet(), FormatGet()));
        return;
      }

      if (tex->RawStrideGet() != Texture::RawBPPGet(tex->FormatGet()) * tex->WidthGet()) {
        EnvironmentGet()->LogError(detail::Format("Stride conversion necessary in TextureBacking::Write - NYI, please make this Texture's stride %d", Texture::RawBPPGet(tex->FormatGet()) * tex->WidthGet()));
        return;
      }

      const unsigned char *data = tex->RawDataGet();
      int stride = tex->RawStrideGet();
      std::vector<unsigned char> dataStorage;

      if (tex->FormatGet() == Texture::FORMAT_RGB_8) {
        // we get to do some conversion! yaaaay.
        int pixels = tex->WidthGet() * tex->HeightGet();
        dataStorage.resize(pixels * Texture::RawBPPGet(Texture::FORMAT_RGBA_8));
        const unsigned char *read = data;
        unsigned char *write = &dataStorage[0];
        
        for (int i = 0; i < pixels; ++i) {
          *write++ = *read++;
          *write++ = *read++;
          *write++ = *read++;
          *write++ = 255;
        }

        data = &dataStorage[0];
        stride = (int)dataStorage.size() / tex->HeightGet(); // yes, this is a wasted div; it's not really going to be an issue, though.
      }

      D3D11_BOX box;
      box.left = sx;
      box.top = sy;
      box.right = sx + tex->WidthGet();
      box.bottom = sy + tex->HeightGet();
      box.front = 0;
      box.back = 1;
      static_cast<RendererDX11*>(EnvironmentGet()->RendererGet())->ContextGet()->UpdateSubresource(m_tex, 0, &box, data, stride, 0);
    }

    RendererDX11::RendererDX11(Environment *env, ID3D11DeviceContext *context) :
      Renderer(env),
      m_context(context),
      m_vs(0),
      m_ps(0),
      m_vertices(0),
      m_verticesLayout(0),
      m_verticesQuadcount(0),
      m_verticesQuadpos(0),
      m_verticesLastQuadsize(0),
      m_verticesLastQuadpos(0),
      m_currentTexture(0)
    {
      CreateBuffers(1 << 16); // maximum size that will fit in a ushort

      // Clear state structures
      memset(&m_rasterizerState, 0, sizeof(m_rasterizerState));

      // Create shaders
      ID3DBlob *vs = 0;
      ID3DBlob *ps = 0;
      if (D3DCompile(sVertexShader, strlen(sVertexShader), 0, 0, 0, 0, "vs_2_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &vs, 0) != S_OK) {
        EnvironmentGet()->LogError("Failure to compile vertex shader");
      }
      if (D3DCompile(sPixelShader, strlen(sPixelShader), 0, 0, 0, 0, "ps_2_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &ps, 0) != S_OK) {
        EnvironmentGet()->LogError("Failure to compile pixel shader");
      }

      if (DeviceGet()->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &m_vs) != S_OK) {
        EnvironmentGet()->LogError("Failure to create vertex shader");
      }
      if (DeviceGet()->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &m_ps) != S_OK) {
        EnvironmentGet()->LogError("Failure to create pixel shader");
      }

      if (DeviceGet()->CreateInputLayout(sLayout, sizeof(sLayout) / sizeof(*sLayout), vs->GetBufferPointer(), vs->GetBufferSize(), &m_verticesLayout) != S_OK) {
        EnvironmentGet()->LogError("Failure to create input layout");
      }

      vs->Release();
      ps->Release();
    };

    RendererDX11::~RendererDX11() {
      m_vs->Release();
      m_ps->Release();
      m_verticesLayout->Release();
      m_vertices->Release();
      m_indices->Release();
    }

    void RendererDX11::Begin(int width, int height) {
      Renderer::Begin(width, height);

      UINT stride = sizeof(Vertex);
      UINT offset = 0;
      ContextGet()->IASetVertexBuffers(0, 1, &m_vertices, &stride, &offset);
      ContextGet()->IASetIndexBuffer(m_indices, DXGI_FORMAT_R16_UINT, 0);
      ContextGet()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      ContextGet()->VSSetShader(m_vs, 0, 0);
      ContextGet()->PSSetShader(m_ps, 0, 0);
      
      D3D11_VIEWPORT viewport;
      memset(&viewport, 0, sizeof(viewport));
      viewport.TopLeftX = 0;
      viewport.TopLeftY = 0;
      viewport.Width = (float)width;
      viewport.Height = (float)height;
      ContextGet()->RSSetViewports(1, &viewport);
    }

    void RendererDX11::End() {
    }

    Renderer::Vertex *RendererDX11::Request(int quads) {
      if (quads > m_verticesQuadcount) {
        EnvironmentGet()->LogError("Exceeded valid quad count in a single draw call; splitting NYI");
        return 0;
      }

      D3D11_MAP mapFlag = D3D11_MAP_WRITE_NO_OVERWRITE;
      if (m_verticesQuadpos + quads > m_verticesQuadcount) {
        // we'll have to clear it out
        m_verticesQuadpos = 0;
        mapFlag = D3D11_MAP_WRITE_DISCARD;
      }

      D3D11_MAPPED_SUBRESOURCE mapData;
      if (m_context->Map(m_vertices, 0, mapFlag, 0, &mapData) != S_OK) {
        EnvironmentGet()->LogError("Error mapping data");
        return 0;
      }

      m_verticesLastQuadpos = m_verticesQuadpos;
      m_verticesLastQuadsize = quads;
      m_verticesQuadpos += quads;

      return (Renderer::Vertex*)mapData.pData + m_verticesLastQuadpos * 4;
    }

    void RendererDX11::Return(int quads /*= -1*/) {
      m_context->Unmap(m_vertices, 0);

      if (quads == -1) quads = m_verticesLastQuadsize;

      m_context->DrawIndexed(quads * 6, 0, m_verticesLastQuadpos * 6);
    }

    TextureBackingPtr RendererDX11::TextureCreate(int width, int height, Texture::Format mode) {
      return TextureBackingPtr(new TextureBackingDX11(EnvironmentGet(), width, height, mode));
    }

    void RendererDX11::TextureSet(const detail::TextureBackingPtr &tex) {
      ID3D11Texture2D *ntex = tex.Get() ? static_cast<TextureBackingDX11*>(tex.Get())->TexGet() : 0;
      if (m_currentTexture != ntex) {
        m_currentTexture = ntex;
        // do what
      }
    }

    void RendererDX11::StatePush() {
      // not even bothering atm
    }

    void RendererDX11::StateClean() {
      // not even bothering atm
    }

    void RendererDX11::StatePop() {
      // not even bothering atm
    }

    ID3D11Device *RendererDX11::DeviceGet() const {
      ID3D11Device *dev = 0;
      ContextGet()->GetDevice(&dev);
      return dev;
    }

    void RendererDX11::ScissorSet(const Rect &rect) {
      D3D11_RECT d3drect;
      d3drect.left = (int)floor(rect.s.x + 0.5f);
      d3drect.top = (int)floor(rect.s.y + 0.5f);
      d3drect.right = (int)floor(rect.e.x + 0.5f);
      d3drect.bottom = (int)floor(rect.e.y + 0.5f);
      ContextGet()->RSSetScissorRects(1, &d3drect);
    }

    void RendererDX11::CreateBuffers(int len) {
      int quadLen = len / 4;

      {
        D3D11_BUFFER_DESC vertexBufferDesc;
        memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = sizeof(Vertex) * quadLen * 4;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags = 0;

        if (DeviceGet()->CreateBuffer(&vertexBufferDesc, 0, &m_vertices) != S_OK) {
          EnvironmentGet()->LogError("Failure to allocate vertex buffer");
        }
      }

      {
        vector<unsigned short> elements(quadLen * 6);
        int writepos = 0;
        for (int i = 0; i < quadLen; ++i) {
          elements[writepos++] = i * 4 + 0;
          elements[writepos++] = i * 4 + 1;
          elements[writepos++] = i * 4 + 3;
          elements[writepos++] = i * 4 + 1;
          elements[writepos++] = i * 4 + 2;
          elements[writepos++] = i * 4 + 3;
        }

        D3D11_BUFFER_DESC indexBufferDesc;
        memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));
        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        indexBufferDesc.ByteWidth = sizeof(unsigned short)* 6;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA indexBufferData;
        memset(&indexBufferData, 0, sizeof(indexBufferData));
        indexBufferData.pSysMem = &elements[0];

        if (DeviceGet()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indices) != S_OK) {
          EnvironmentGet()->LogError("Failure to allocate index buffer");
        }
      }
      
      m_verticesQuadcount = quadLen;
      m_verticesQuadpos = m_verticesQuadcount; // will force an array rebuild
    }
  }
}

