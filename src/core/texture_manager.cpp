
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
      // register ourselves in the texture manager so it can manipulate us as needed
      m_env->GetRenderer()->BackingInit(this);

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
      m_env->GetRenderer()->BackingShutdown(this);

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

    TextureChunk::TextureChunk() :
        m_texture_width(0),
        m_texture_height(0),
        m_bounds(0, 0, 0, 0)
    {
    }

    TextureChunk::~TextureChunk() {
      m_backing->m_env->TextureChunkShutdown(this);
    }

    /*static*/ TextureChunkPtr TextureChunk::Create() {
      return TextureChunkPtr(new TextureChunk());
    }

    void TextureChunk::Attach(const TextureBackingPtr &backing, int sx, int sy, int ex, int ey) {
      m_backing = backing;
      m_texture_width = ex - sx;
      m_texture_height = ey - sy;
      m_bounds.s.x = (float)sx / backing->m_surface_width;
      m_bounds.s.y = (float)sy / backing->m_surface_width;
      m_bounds.e.x = m_bounds.s.x + (float)m_texture_width / backing->m_surface_width;
      m_bounds.e.y = m_bounds.s.y + (float)m_texture_height / backing->m_surface_height;
    }

    TextureManager::TextureManager(Environment *env) : m_env(env) {
    }

    TextureManager::~TextureManager() {
      // Backings *should* have cleaned themselves up already
      if (!m_backing.empty()) {
        m_env->LogError("Texture backing failed at cleanup; attempting to recover");

        for (std::set<TextureBacking *>::iterator itr = m_backing.begin(); itr != m_backing.end();) {
          std::set<TextureBacking *>::iterator t = itr;
          ++itr;

          delete *t; // will delete itself from this member
        }
      }
    }

    TextureBackingPtr TextureManager::BackingCreate(int width, int height, int mode) {
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

      TextureBackingPtr backing(new TextureBacking(m_env));

      backing->Allocate(width, height, modeGL);

      return backing;
    }

    void TextureManager::BackingInit(TextureBacking *backing) {
      m_backing.insert(backing);
    }

    void TextureManager::BackingShutdown(TextureBacking *backing) {
      m_backing.erase(backing);
    }
  }
}

