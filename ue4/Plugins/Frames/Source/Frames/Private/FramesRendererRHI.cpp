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

#include "FramesPCH.h"

#include "FramesRendererRHI.h"

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
    class CfgRendererRHI : public Renderer {
    public:
      virtual detail::Renderer *Create(Environment *env) const {
        return new detail::RendererRHI(env);
      }
    };

    Configuration::RendererPtr Configuration::RendererRHI() {
      return Configuration::RendererPtr(new CfgRendererRHI());
    }
  }

  namespace detail {
    //The input-layout description
    /*static const D3D11_INPUT_ELEMENT_DESC sLayout[] =
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Renderer::Vertex, p), D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Renderer::Vertex, t), D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Renderer::Vertex, c), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    static const char sShader[] =
      "cbuffer Size : register(b0) { float width : packoffset(c0.x); float height : packoffset(c0.y); };\n"  // size of screen; changes rarely
      "cbuffer Item : register(b1) { int sampleMode : packoffset(c0); };\n"  // 0 means don't sample. 1 means do sample and multiply. 2 means do sample and multiply; pretend .rgb is 1.f.
      "Texture2D <float4> sprite : register(t0);\n"  // sprite texture to reference if it is being referenced
      "SamplerState spriteSample : register(s0);\n" // sampler to use for sprite texture
      "\n"
      "struct VIn { float2 position : POSITION; float2 tex : TEXCOORD0; float4 color : COLOR; };\n"
      "struct VOut { float4 position : SV_POSITION; float2 tex : TEXCOORD0; float4 color : COLOR; };\n"
      "VOut VS(VIn input) { VOut output; float2 cp = input.position; cp.x /= width; cp.y /= -height; cp *= 2; cp += float2(-1.f, 1.f); output.position = float4(cp, 0.f, 1.f); output.tex = input.tex; output.color = input.color; return output; }\n"
      "float4 PS(VOut input) : SV_TARGET { if (sampleMode == 1) input.color *= sprite.Sample(spriteSample, input.tex); if (sampleMode == 2) input.color.a *= sprite.Sample(spriteSample, input.tex).r; return input.color; }\n";
      */

    TextureBackingRHI::TextureBackingRHI(Environment *env, int width, int height, Texture::Format format) : TextureBacking(env, width, height, format) {
      m_tex = RHICreateTexture2D(width, height, (format), 1, 1, TexCreate_ShaderResource, 0);
    }

    TextureBackingRHI::~TextureBackingRHI() {
      // toot
    }

    #if 0
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
      static_cast<RendererDX11*>(Renderer::GetFrom(EnvironmentGet()))->ContextGet()->UpdateSubresource(m_tex, 0, &box, data, stride, 0);
    }

    RendererDX11::RendererDX11(Environment *env, ID3D11DeviceContext *context) :
      Renderer(env),
      m_device(0),
      m_context(context),
      m_rasterizerState(0),
      m_blendState(0),
      m_depthState(0),
      m_vs(0),
      m_ps(0),
      m_shader_ci_size(0),  // hardcoded for now
      m_shader_ci_item(1),  // hardcoded for now
      m_shader_tex(0),  // hardcoded for now
      m_shader_sample(0),  // hardcoded for now
      m_shader_ci_size_buffer(0),
      m_shader_ci_item_buffer_sample_off(0),
      m_shader_ci_item_buffer_sample_full(0),
      m_shader_ci_item_buffer_sample_alpha(0),
      m_sampler(0),
      m_vertices(0),
      m_verticesLayout(0),
      m_verticesQuadcount(0),
      m_verticesQuadpos(0),
      m_verticesLastQuadsize(0),
      m_verticesLastQuadpos(0),
      m_indices(0),
      m_currentTexture(0)
    {
      m_context->AddRef();  // we're storing this value, so let's add a reference to it
      m_context->GetDevice(&m_device);

      CreateBuffers(1 << 16); // maximum size that will fit in a ushort

      // Create shaders
      ID3DBlob *vs = 0;
      ID3DBlob *ps = 0;
      ID3DBlob *errors = 0;
      if (D3DCompile(sShader, strlen(sShader), 0, 0, 0, "VS", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &vs, &errors) != S_OK) {
        EnvironmentGet()->LogError("Failure to compile vertex shader");
        EnvironmentGet()->LogError("Err: " + std::string((const char *)errors->GetBufferPointer(), (const char *)errors->GetBufferPointer() + errors->GetBufferSize()));
        errors->Release();
      }
      if (D3DCompile(sShader, strlen(sShader), 0, 0, 0, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &ps, &errors) != S_OK) {
        EnvironmentGet()->LogError("Failure to compile pixel shader");
        EnvironmentGet()->LogError("Err: " + std::string((const char *)errors->GetBufferPointer(), (const char *)errors->GetBufferPointer() + errors->GetBufferSize()));
        errors->Release();
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

      // Create size buffer
      {
        // TODO: make this immutable when possible
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.ByteWidth = 16;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        if (DeviceGet()->CreateBuffer(&desc, 0, &m_shader_ci_size_buffer) != S_OK) {
          EnvironmentGet()->LogError("Failure to create size buffer");
        }
      }

      // Create item buffers
      {
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.ByteWidth = 16;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        int flag[4] = { 0, };

        D3D11_SUBRESOURCE_DATA data;
        memset(&data, 0, sizeof(data));
        data.pSysMem = flag;
        
        if (DeviceGet()->CreateBuffer(&desc, &data, &m_shader_ci_item_buffer_sample_off) != S_OK) {
          EnvironmentGet()->LogError("Failure to create disabled sample flag");
        }

        flag[0] = 1;
        if (DeviceGet()->CreateBuffer(&desc, &data, &m_shader_ci_item_buffer_sample_full) != S_OK) {
          EnvironmentGet()->LogError("Failure to create enabled sample flag");
        }

        flag[0] = 2;
        if (DeviceGet()->CreateBuffer(&desc, &data, &m_shader_ci_item_buffer_sample_alpha) != S_OK) {
          EnvironmentGet()->LogError("Failure to create enabled sample flag");
        }
      }

      // Create states
      {
        D3D11_SAMPLER_DESC ss;
        memset(&ss, 0, sizeof(ss));
        ss.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        ss.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        ss.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        ss.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        ss.MipLODBias = 0.f;
        ss.MaxAnisotropy = 1;
        ss.MinLOD = 0.f;
        ss.MaxLOD = D3D11_FLOAT32_MAX;
        DeviceGet()->CreateSamplerState(&ss, &m_sampler);
      }

      {
        D3D11_RASTERIZER_DESC rs;
        memset(&rs, 0, sizeof(rs));
        rs.FillMode = D3D11_FILL_SOLID;
        rs.CullMode = D3D11_CULL_NONE;
        rs.ScissorEnable = true;
        rs.MultisampleEnable = true;
        rs.AntialiasedLineEnable = true;
        DeviceGet()->CreateRasterizerState(&rs, &m_rasterizerState);
      }

      {
        D3D11_BLEND_DESC bs;
        memset(&bs, 0, sizeof(bs));
        bs.RenderTarget[0].BlendEnable = true;
        bs.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        bs.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        bs.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bs.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        bs.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        bs.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        DeviceGet()->CreateBlendState(&bs, &m_blendState);
      }

      {
        D3D11_DEPTH_STENCIL_DESC ds;
        memset(&ds, 0, sizeof(ds));
        ds.DepthEnable = false;
        DeviceGet()->CreateDepthStencilState(&ds, &m_depthState);
      }
    }

    RendererDX11::~RendererDX11() {
      if (m_rasterizerState) {
        m_rasterizerState->Release();
      }

      if (m_blendState) {
        m_blendState->Release();
      }

      if (m_depthState) {
        m_depthState->Release();
      }

      if (m_vs) {
        m_vs->Release();
      }

      if (m_ps) {
        m_ps->Release();
      }

      if (m_shader_ci_size_buffer) {
        m_shader_ci_size_buffer->Release();
      }

      if (m_shader_ci_item_buffer_sample_off) {
        m_shader_ci_item_buffer_sample_off->Release();
      }

      if (m_shader_ci_item_buffer_sample_full) {
        m_shader_ci_item_buffer_sample_full->Release();
      }

      if (m_shader_ci_item_buffer_sample_alpha) {
        m_shader_ci_item_buffer_sample_alpha->Release();
      }

      if (m_sampler) {
        m_sampler->Release();
      }

      if (m_verticesLayout) {
        m_verticesLayout->Release();
      }

      if (m_vertices) {
        m_vertices->Release();
      }

      if (m_indices) {
        m_indices->Release();
      }

      if (m_context) {
        m_context->Release();
      }

      if (m_device) {
        m_device->Release();
      }
    }

    void RendererDX11::Begin(int width, int height) {
      Renderer::Begin(width, height);

      m_currentTexture = 0;

      {
        D3D11_MAPPED_SUBRESOURCE map;
        // TODO: don't update if width/height didn't change
        if (ContextGet()->Map(m_shader_ci_size_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map) != S_OK) {
          EnvironmentGet()->LogError("Failure to update size buffer");
        } else {
          float *dat = (float*)map.pData;
          dat[0] = (float)width;
          dat[1] = (float)height;
          ContextGet()->Unmap(m_shader_ci_size_buffer, 0);
        }
      }

      UINT stride = sizeof(Vertex);
      UINT offset = 0;
      ContextGet()->IASetVertexBuffers(0, 1, &m_vertices, &stride, &offset);
      ContextGet()->IASetIndexBuffer(m_indices, DXGI_FORMAT_R16_UINT, 0);
      ContextGet()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      ContextGet()->IASetInputLayout(m_verticesLayout);
      ContextGet()->VSSetShader(m_vs, 0, 0);
      ContextGet()->VSSetConstantBuffers(m_shader_ci_size, 1, &m_shader_ci_size_buffer);
      ContextGet()->PSSetShader(m_ps, 0, 0);
      ContextGet()->PSSetSamplers(m_shader_sample, 1, &m_sampler);
      ContextGet()->PSSetConstantBuffers(m_shader_ci_item, 1, &m_shader_ci_item_buffer_sample_off);
      ContextGet()->RSSetState(m_rasterizerState);
      ContextGet()->OMSetBlendState(m_blendState, 0, ~0);
      ContextGet()->OMSetDepthStencilState(m_depthState, 0);
      
      D3D11_VIEWPORT viewport;
      memset(&viewport, 0, sizeof(viewport));
      viewport.TopLeftX = 0;
      viewport.TopLeftY = 0;
      viewport.Width = (float)width;
      viewport.Height = (float)height;
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
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

      m_context->DrawIndexed(quads * 6, 0, m_verticesLastQuadpos * 4);
    }

    TextureBackingPtr RendererDX11::TextureCreate(int width, int height, Texture::Format mode) {
      return TextureBackingPtr(new TextureBackingDX11(EnvironmentGet(), width, height, mode));
    }

    void RendererDX11::TextureSet(const detail::TextureBackingPtr &tex) {
      TextureBackingDX11 *backing = tex.Get() ? static_cast<TextureBackingDX11*>(tex.Get()) : 0;
      ID3D11ShaderResourceView *ntex = backing ? backing->ShaderResourceViewGet() : 0;
      if (m_currentTexture != ntex) {
        m_currentTexture = ntex;
        
        if (m_currentTexture) {
          if (backing->FormatGet() == Texture::FORMAT_R_8) {
            ContextGet()->PSSetConstantBuffers(m_shader_ci_item, 1, &m_shader_ci_item_buffer_sample_alpha);
          } else {
            ContextGet()->PSSetConstantBuffers(m_shader_ci_item, 1, &m_shader_ci_item_buffer_sample_full);
          }
          ContextGet()->PSSetShaderResources(m_shader_tex, 1, &m_currentTexture);
        } else {
          ContextGet()->PSSetConstantBuffers(m_shader_ci_item, 1, &m_shader_ci_item_buffer_sample_off);
        }
      }
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
        indexBufferDesc.ByteWidth = sizeof(unsigned short) * quadLen * 6;
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
  #endif
  }
}
