
#include "frames/texture_manager.h"

#include "frames/configuration.h"
#include "frames/environment.h"
#include "frames/texture_config.h"

namespace Frames {
  TextureBacking::TextureBacking() :
      m_env(0),
      m_id(0),
      m_surface_width(0),
      m_surface_height(0),
      m_refs(0)
  {
  }

  TextureBacking::~TextureBacking() {
    m_env->GetTextureManager()->Internal_Shutdown_Backing(this);
    if (m_refs) {
      m_env->LogError("Leak in texture backing");
    }

    glDeleteTextures(1, &m_id);
  }

  TextureChunk::TextureChunk() :
      m_texture_width(0),
      m_texture_height(0),
      m_sx(0),
      m_sy(0),
      m_ex(0),
      m_ey(0),
      m_refs(0)
  {
  }

  TextureChunk::~TextureChunk() {
    m_backing->m_env->GetTextureManager()->Internal_Shutdown_Chunk(this);
    if (m_refs) {
      m_backing->m_env->LogError("Leak in texture chunks");
    }
  }

  TextureManager::TextureManager(Environment *env) : m_env(env) {
  }
  TextureManager::~TextureManager() {
    // First, nuke chunks
    for (std::map<std::string, TextureChunk *>::iterator itr = m_texture.begin(); itr != m_texture.end(); ) {
      std::map<std::string, TextureChunk *>::iterator t = itr;
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
    if (m_texture.count(id)) {
      return m_texture[id];
    }

    m_env->LogDebug(Utility::Format("Attempting to load texture %s", id.c_str()));

    TextureConfig conf = m_env->GetConfiguration().textureFromId->Create(m_env, id);

    if (conf.GetMode() == TextureConfig::RAW) {
      // time to GL-ize it
      // for now we're just putting it into its own po2 texture

      glEnable(GL_TEXTURE_2D);

      GLuint tex;
      glGenTextures(1, &tex);
      if (!tex) {
        // whoops
        m_env->LogError(Utility::Format("Failure to allocate room for texture %s", id.c_str()));
        return 0;
      }

      TextureBacking *backing = new TextureBacking();
      backing->m_env = m_env;
      backing->m_surface_width = Utility::ClampToPowerOf2(conf.GetWidth());
      backing->m_surface_height = Utility::ClampToPowerOf2(conf.GetHeight());
      backing->m_id = tex;
      
      m_backing.insert(backing);

      TextureChunk *chunk = new TextureChunk();
      chunk->m_backing = backing;
      chunk->m_texture_width = conf.GetWidth();
      chunk->m_texture_height = conf.GetHeight();
      chunk->m_sx = 0;
      chunk->m_sy = 0;
      chunk->m_ex = (float)chunk->m_texture_width / backing->m_surface_width;
      chunk->m_ey = (float)chunk->m_texture_height / backing->m_surface_height;

      m_texture[id] = chunk;
      m_texture_reverse[chunk] = id;

      glBindTexture(GL_TEXTURE_2D, tex);

      int gl_tex_mode = GL_RGBA;
      int input_tex_mode = GL_RGBA;

      if (conf.Raw_GetType() == TextureConfig::MODE_RGBA) {
        gl_tex_mode = GL_RGBA;
        input_tex_mode = GL_RGBA;
      } else if (conf.Raw_GetType() == TextureConfig::MODE_RGB) {
        gl_tex_mode = GL_RGBA;
        input_tex_mode = GL_RGB;
      } else if (conf.Raw_GetType() == TextureConfig::MODE_L) {
        gl_tex_mode = GL_LUMINANCE;
        input_tex_mode = GL_LUMINANCE;
      } else if (conf.Raw_GetType() == TextureConfig::MODE_A) {
        gl_tex_mode = GL_ALPHA;
        input_tex_mode = GL_ALPHA;
      } else {
        m_env->LogError(Utility::Format("Unrecognized raw type %d in texture %s", conf.Raw_GetType(), id.c_str()));
        return chunk;
      }

      int bpp = TextureConfig::GetBPP(conf.Raw_GetType());

      glTexImage2D(GL_TEXTURE_2D, 0, gl_tex_mode, backing->m_surface_width, backing->m_surface_height, 0, input_tex_mode, GL_UNSIGNED_BYTE, 0);

      if (conf.Raw_GetStride() == bpp * conf.GetWidth()) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, chunk->m_texture_width, chunk->m_texture_height, input_tex_mode, GL_UNSIGNED_BYTE, conf.Raw_GetData());
      } else {
        for (int y = 0; y < conf.GetHeight(); ++y) {
          glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, chunk->m_texture_width, 1, input_tex_mode, GL_UNSIGNED_BYTE, conf.Raw_GetData() + y * conf.Raw_GetStride());
        }
      }

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      return chunk;
    } else {
      m_env->LogError("Unknown texture config type");
      return 0;
    }
  }

  void TextureManager::Internal_Shutdown_Backing(TextureBacking *backing) {
    m_backing.erase(backing); // easy peasy
  }
  void TextureManager::Internal_Shutdown_Chunk(TextureChunk *chunk) {
    m_texture.erase(m_texture_reverse[chunk]);
    m_texture_reverse.erase(chunk);
  }
}

