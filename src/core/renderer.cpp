
#include "frames/renderer.h"

#include "frames/detail.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
#include "frames/rect.h"
#include "frames/texture.h"

#include "boost/static_assert.hpp"

#include "frames/os_gl.h"

#include <vector>
#include <algorithm>

using namespace std;

namespace Frames {
  namespace detail {
    TextureBacking::TextureBacking(Environment *env) :
      m_env(env),
      m_id(0),
      m_surface_width(0),
      m_surface_height(0),
      m_alloc_next_x(0),
      m_alloc_cur_y(0),
      m_alloc_next_y(0)
    {
      glGenTextures(1, &m_id);
      if (!m_id) {
        // whoops
        m_env->LogError(detail::Format("Failure to allocate room for texture"));
      }

      // setup standard texture parameters
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, GlidGet());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
    }

    TextureBacking::~TextureBacking() {
      glDeleteTextures(1, &m_id);
    }

    void TextureBacking::Allocate(int width, int height, int gltype) {
      m_surface_width = width;
      m_surface_height = height;
      glBindTexture(GL_TEXTURE_2D, m_id);
      glTexImage2D(GL_TEXTURE_2D, 0, gltype, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); // I'm assuming the last three values are irrelevant
    }

    std::pair<int, int> TextureBacking::AllocateSubtexture(int width, int height) {
      // very very bad allocation, but simple for prototyping
      if (m_alloc_next_x + width > m_surface_width) {
        m_alloc_next_x = 0;
        m_alloc_cur_y = m_alloc_next_y;
      }
      m_alloc_next_y = std::max(m_alloc_next_y, m_alloc_cur_y + height);

      int tx = m_alloc_next_x;
      m_alloc_next_x += width;

      if (m_alloc_cur_y + height > m_surface_height || width > m_surface_width) {
        m_env->LogError("Out of space for allocating subtexture");
      }

      return std::make_pair(tx, m_alloc_cur_y);
    }

    void TextureBacking::Write(int sx, int sy, const TexturePtr &tex) {
      int input_tex_mode;
      if (tex->FormatGet() == Texture::FORMAT_RGBA_8) {
        input_tex_mode = GL_RGBA;
      } else if (tex->FormatGet() == Texture::FORMAT_RGB_8) {
        input_tex_mode = GL_RGB;
      } else if (tex->FormatGet() == Texture::FORMAT_L_8) {
        input_tex_mode = GL_LUMINANCE;
      } else if (tex->FormatGet() == Texture::FORMAT_A_8) {
        input_tex_mode = GL_ALPHA;
      } else {
        m_env->LogError(detail::Format("Unrecognized raw type %d in texture", tex->FormatGet()));
        return;
      }

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, GlidGet());
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if (tex->RawStrideGet() == Texture::RawBPPGet(tex->FormatGet()) * tex->WidthGet()) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, sx, sy, tex->WidthGet(), tex->HeightGet(), input_tex_mode, GL_UNSIGNED_BYTE, tex->RawDataGet());
      } else {
        for (int y = 0; y < tex->HeightGet(); ++y) {
          glTexSubImage2D(GL_TEXTURE_2D, 0, sx, sy + y, tex->WidthGet(), 1, input_tex_mode, GL_UNSIGNED_BYTE, tex->RawDataGet() + y * tex->RawStrideGet());
        }
      }
    }

    Renderer::Renderer(Environment *env) :
        m_env(env)
    {
      // prime our alpha stack
      m_alpha.push_back(1);
    }

    Renderer::~Renderer() { }

    void Renderer::Begin(int width, int height) {
      m_width = width;
      m_height = height;
    }

    void Renderer::End() {
      if (!m_scissor.empty()) {
        EnvironmentGet()->LogError("Mismatched scissor push/pop at end of frame.");
        while (!m_scissor.empty()) {
          m_scissor.pop();
        }
      }
    }

    void Renderer::ScissorPush(Rect rect) {
      if (!m_scissor.empty()) {
        // Create the intersection of scissors
        rect.s.x = max(rect.s.x, m_scissor.top().s.x);
        rect.s.y = max(rect.s.y, m_scissor.top().s.y);
        rect.e.x = min(rect.e.x, m_scissor.top().e.x);
        rect.e.y = min(rect.e.y, m_scissor.top().e.y);
        if (rect.s.x >= rect.e.x || rect.s.y >= rect.e.y) {
          // degenerate scissor, don't render anything (todo: *actually* don't render anything?)
          rect = Rect(0.f, 0.f, 0.f, 0.f);
        }
      }

      m_scissor.push(rect);

      ScissorSet(rect);
    }

    void Renderer::ScissorPop() {
      if (m_scissor.empty()) {
        EnvironmentGet()->LogError("Excessive scissor popping");
        return;
      }

      m_scissor.pop();

      if (m_scissor.empty()) {
        ScissorSet(Rect(0, 0, (float)m_width, (float)m_height));
      } else {
        ScissorSet(m_scissor.top());
      }
    }

    void Renderer::AlphaPush(float alpha) {
      m_alpha.push_back(alpha * AlphaGet());
    }

    float Renderer::AlphaGet() const {
      return m_alpha.back();
    }

    void Renderer::AlphaPop() {
      m_alpha.pop_back();
    }

    bool Renderer::WriteCroppedRect(Vertex *verts, const Rect &screen, const Color &color, const Rect &bounds) {
      if (screen.s.x > bounds.e.x || screen.e.x < bounds.s.x || screen.s.y > bounds.e.y || screen.e.y < bounds.s.y) {
        return false;
      }

      // set up boundaries
      verts[0].p = detail::Clamp(screen.s, bounds.s, bounds.e);
      verts[2].p = detail::Clamp(screen.e, bounds.s, bounds.e);

      // spread it out
      verts[1].p.x = verts[2].p.x;
      verts[1].p.y = verts[0].p.y;
      verts[3].p.x = verts[0].p.x;
      verts[3].p.y = verts[2].p.y;

      // colorize
      verts[0].c = color;
      verts[1].c = color;
      verts[2].c = color;
      verts[3].c = color;

      return true;
    }

    bool Renderer::WriteCroppedTexRect(Vertex *vertex, const Rect &screen, const Rect &tex, const Color &color, const Rect &bounds) {
      if (screen.s.x > bounds.e.x || screen.e.x < bounds.s.x || screen.s.y > bounds.e.y || screen.e.y < bounds.s.y) {
        return false;
      }
    
      // generate [0] and [2], derive the others from that
      if (screen.s.x >= bounds.s.x && screen.e.x <= bounds.e.x && screen.s.y >= bounds.s.y && screen.e.y <= bounds.e.y) {
        // easy solution, we're fully within bounds
        vertex[0].p = screen.s;
        vertex[0].t = tex.s;
    
        vertex[2].p = screen.e;
        vertex[2].t = tex.e;
      } else {
        // we intersect, so we need to do all the work
        vertex[0].p.x = std::max(screen.s.x, bounds.s.x);
        vertex[0].p.y = std::max(screen.s.y, bounds.s.y);
      
        vertex[2].p.x = std::min(screen.e.x, bounds.e.x);
        vertex[2].p.y = std::min(screen.e.y, bounds.e.y);
      
        float xs = (tex.e.x - tex.s.x) / (screen.e.x - screen.s.x);
        float ys = (tex.e.y - tex.s.y) / (screen.e.y - screen.s.y);
      
        vertex[0].t.x = (vertex[0].p.x - screen.s.x) * xs + tex.s.x;
        vertex[0].t.y = (vertex[0].p.y - screen.s.y) * ys + tex.s.y;
      
        vertex[2].t.x = (vertex[2].p.x - screen.s.x) * xs + tex.s.x;
        vertex[2].t.y = (vertex[2].p.y - screen.s.y) * ys + tex.s.y;
      }
    
      // now we've got a valid [0] and [2], make textures
      vertex[1].p.x = vertex[2].p.x;
      vertex[1].p.y = vertex[0].p.y;
    
      vertex[3].p.x = vertex[0].p.x;
      vertex[3].p.y = vertex[2].p.y;
    
      vertex[1].t.x = vertex[2].t.x;
      vertex[1].t.y = vertex[0].t.y;
    
      vertex[3].t.x = vertex[0].t.x;
      vertex[3].t.y = vertex[2].t.y;
    
      // copy all colors over
      vertex[0].c = color;
      vertex[1].c = color;
      vertex[2].c = color;
      vertex[3].c = color;
    
      return true;
    }

    TextureBackingPtr Renderer::BackingCreate(int width, int height, int mode) {
      int modeGL;
      if (mode == Texture::FORMAT_RGBA_8) {
        modeGL = GL_RGBA;
      } else if (mode == Texture::FORMAT_RGB_8) {
        modeGL = GL_RGBA;
      } else if (mode == Texture::FORMAT_L_8) {
        modeGL = GL_LUMINANCE;
      } else if (mode == Texture::FORMAT_A_8) {
        modeGL = GL_ALPHA;
      } else {
        m_env->LogError(detail::Format("Unrecognized raw type %d in texture", mode));
        return detail::TextureBackingPtr();
      }

      TextureBackingPtr backing = TextureCreate();

      backing->Allocate(width, height, modeGL);

      return backing;
    }
  }
}

