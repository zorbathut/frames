
#include "frames/texture_manager.h"

#include "frames/configuration.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
#include "frames/renderer.h"
#include "frames/texture.h"

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
  }
}

