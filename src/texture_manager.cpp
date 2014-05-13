
#include "frames/texture_manager.h"

#include "frames/configuration.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
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
    }

    TextureBacking::~TextureBacking() {
      m_env->GetTextureManager()->Internal_Shutdown_Backing(this);

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

    TextureChunk::TextureChunk() :
        m_texture_width(0),
        m_texture_height(0),
        m_bounds(0, 0, 0, 0)
    {
    }

    TextureChunk::~TextureChunk() {
      m_backing->m_env->GetTextureManager()->Internal_Shutdown_Chunk(this);
    }

    TextureManager::TextureManager(Environment *env) : m_env(env) {
    }
    TextureManager::~TextureManager() {
      // First, nuke chunks
      for (boost::bimap<std::string, TextureChunk *>::left_iterator itr = m_texture.left.begin(); itr != m_texture.left.end(); ) {
        boost::bimap<std::string, TextureChunk *>::left_iterator t = itr;
        ++itr;

        // now we can go
        delete t->second; // will clean itself up
      }

      // Then, nuke backings
      for (std::set<TextureBacking *>::iterator itr = m_backing.begin(); itr != m_backing.end(); ) {
        std::set<TextureBacking *>::iterator t = itr;
        ++itr;

        // now we can go
        delete *t; // will clean itself up
      }
    }

    TextureChunkPtr TextureManager::TextureFromId(const std::string &id) {
      if (m_texture.left.count(id)) {
        return TextureChunkPtr(m_texture.left.find(id)->second);
      }

      m_env->LogDebug(detail::Format("Attempting to load texture %s", id));

      TexturePtr conf = m_env->ConfigurationGet().TextureFromIdGet()->Create(m_env, id);

      TextureChunkPtr rv = TextureFromConfig(conf);

      if (!rv) {
        m_env->LogError(detail::Format("Failed to load texture %s", id));
        return rv; // something went wrong
      }

      m_texture.insert(boost::bimap<std::string, TextureChunk *>::value_type(id, rv.Get()));

      return rv;
    }

    TextureChunkPtr TextureManager::TextureFromConfig(const TexturePtr &tex, TextureBackingPtr in_backing /*= 0*/) {
      if (tex->TypeGet() == Texture::RAW) {
        // time to GL-ize it
        // for now we're just putting it into its own po2 texture

        glEnable(GL_TEXTURE_2D);

        TextureBackingPtr backing;

        int gl_tex_mode = GL_RGBA;
        int input_tex_mode = GL_RGBA;

        if (tex->FormatGet() == Texture::FORMAT_RGBA_8) {
          gl_tex_mode = GL_RGBA;
          input_tex_mode = GL_RGBA;
        } else if (tex->FormatGet() == Texture::FORMAT_RGB_8) {
          gl_tex_mode = GL_RGBA;
          input_tex_mode = GL_RGB;
        } else if (tex->FormatGet() == Texture::FORMAT_L_8) {
          gl_tex_mode = GL_LUMINANCE;
          input_tex_mode = GL_LUMINANCE;
        } else if (tex->FormatGet() == Texture::FORMAT_A_8) {
          gl_tex_mode = GL_ALPHA;
          input_tex_mode = GL_ALPHA;
        } else {
          m_env->LogError(detail::Format("Unrecognized raw type %d in texture", tex->FormatGet()));
          return TextureChunkPtr();
        }

        if (in_backing) {
          backing = in_backing;
        } else {
          backing.Reset(new TextureBacking(m_env));

          // currently ignoring power-of-two textures
          //backing->Allocate(detail::ClampToPowerOf2(tex->WidthGet()), detail::ClampToPowerOf2(tex->HeightGet()), gl_tex_mode);
          backing->Allocate(tex->WidthGet(), tex->HeightGet(), gl_tex_mode);

          m_backing.insert(backing.Get());  // "safe", because the backing stores enough data that it will clean this up when it's destroyed
        }

        std::pair<int, int> origin = backing->AllocateSubtexture(tex->WidthGet(), tex->HeightGet());

        //m_env->LogDebug(detail::Format("Allocating %d/%d to %d/%d", tex->WidthGet(), tex->HeightGet(), origin.first, origin.second));

        TextureChunkPtr chunk(new TextureChunk());
        chunk->m_backing = backing;
        chunk->m_texture_width = tex->WidthGet();
        chunk->m_texture_height = tex->HeightGet();
        chunk->m_bounds.s.x = (float)origin.first / backing->m_surface_width;
        chunk->m_bounds.s.y = (float)origin.second / backing->m_surface_width;
        chunk->m_bounds.e.x = chunk->m_bounds.s.x + (float)chunk->m_texture_width / backing->m_surface_width;
        chunk->m_bounds.e.y = chunk->m_bounds.s.y + (float)chunk->m_texture_height / backing->m_surface_height;

        glBindTexture(GL_TEXTURE_2D, backing->GetGLID());
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        if (tex->RawStrideGet() == Texture::RawBPPGet(tex->FormatGet()) * tex->WidthGet()) {
          glTexSubImage2D(GL_TEXTURE_2D, 0, origin.first, origin.second, chunk->m_texture_width, chunk->m_texture_height, input_tex_mode, GL_UNSIGNED_BYTE, tex->RawDataGet());
        } else {
          for (int y = 0; y < tex->HeightGet(); ++y) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, origin.first, origin.second + y, chunk->m_texture_width, 1, input_tex_mode, GL_UNSIGNED_BYTE, tex->RawDataGet() + y * tex->RawStrideGet());
          }
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);

        return chunk;
      } else {
        m_env->LogError("Unknown texture type");
        return TextureChunkPtr();
      }
    }

    TextureBackingPtr TextureManager::BackingCreate(int width, int height, int modeGL) {
      TextureBackingPtr backing(new TextureBacking(m_env));

      backing->Allocate(width, height, modeGL);

      m_backing.insert(backing.Get());

      return backing;
    }

    void TextureManager::Internal_Shutdown_Backing(TextureBacking *backing) {
      m_backing.erase(backing); // easy peasy
    }
    void TextureManager::Internal_Shutdown_Chunk(TextureChunk *chunk) {
      if (m_texture.right.count(chunk)) {
        m_texture.right.erase(chunk);
      }
    }
  }
}

