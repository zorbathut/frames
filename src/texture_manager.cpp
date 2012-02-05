
#include "frames/texture_manager.h"

#include "frames/configuration.h"
#include "frames/environment.h"

namespace Frames {
  TextureManager::TextureManager(Environment *env) : m_env(env) { };
  TextureManager::~TextureManager() { };

  TextureManager::Texture::Texture() : id(0), texture_width(0), texture_height(0), surface_width(0), surface_height(0), sx(0), sy(0), ex(0), ey(0) { };
  TextureManager::Texture::~Texture() { glDeleteTextures(1, &id); }

  TextureManager::Texture *TextureManager::TextureFromId(const std::string &id) {
    if (m_textures.count(id)) {
      return &m_textures[id];
    }

    m_env->LogDebug(Utility::Format("Attempting to load texture %s", id.c_str()));

    TextureInfo tinfo = m_env->GetConfiguration().textureFromId->Create(m_env, id);

    if (tinfo.mode == TextureInfo::RAW) {
      // time to GL-ize it
      // for now we're just putting it into its own po2 texture

      TextureInfo glt;
      glt.mode = TextureInfo::GL;
      glt.texture_width = tinfo.texture_width;
      glt.texture_height = tinfo.texture_height;

      glt.gl.surface_width = Utility::ClampToPowerOf2(glt.texture_width);
      glt.gl.surface_height = Utility::ClampToPowerOf2(glt.texture_height);

      glEnable(GL_TEXTURE_2D);

      glGenTextures(1, &glt.gl.id);
      if (!glt.gl.id) {
        // whoops
        m_env->LogError(Utility::Format("Failure to allocate room for texture %s", id.c_str()));
        tinfo.raw.Deallocate(m_env);
        return 0;
      }

      glBindTexture(GL_TEXTURE_2D, glt.gl.id);

      int gl_tex_mode = GL_RGBA;
      int input_tex_mode = GL_RGBA;

      if (tinfo.raw.type == TextureInfo::RAW_RGBA) {
        gl_tex_mode = GL_RGBA;
        input_tex_mode = GL_RGBA;
      } else if (tinfo.raw.type == TextureInfo::RAW_RGB) {
        gl_tex_mode = GL_RGBA;
        input_tex_mode = GL_RGB;
      } else if (tinfo.raw.type == TextureInfo::RAW_L) {
        gl_tex_mode = GL_LUMINANCE;
        input_tex_mode = GL_LUMINANCE;
      } else if (tinfo.raw.type == TextureInfo::RAW_A) {
        gl_tex_mode = GL_ALPHA;
        input_tex_mode = GL_ALPHA;
      } else {
        m_env->LogError(Utility::Format("Unrecognized raw type %d in texture %s", tinfo.raw.type, id.c_str()));
        tinfo.raw.Deallocate(m_env);
        return 0;
      }

      int bpp = TextureInfo::InfoRaw::GetBPP(tinfo.raw.type);

      glTexImage2D(GL_TEXTURE_2D, 0, gl_tex_mode, glt.gl.surface_width, glt.gl.surface_height, 0, input_tex_mode, GL_UNSIGNED_BYTE, 0);

      if (tinfo.raw.stride == bpp * tinfo.texture_width && false) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, glt.texture_width, glt.texture_height, input_tex_mode, GL_UNSIGNED_BYTE, tinfo.raw.data);
      } else {
        for (int y = 0; y < tinfo.texture_height; ++y) {
          glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, glt.texture_width, 1, input_tex_mode, GL_UNSIGNED_BYTE, (unsigned char *)tinfo.raw.data + y * tinfo.raw.stride);
        }
      }

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      tinfo.raw.Deallocate(m_env);

      tinfo = glt;
    }

    if (tinfo.mode != TextureInfo::GL) {
      // whoops
      m_env->LogError(Utility::Format("Received texture with type %d, expected type %d", tinfo.mode, TextureInfo::GL));
      return 0;
    }

    if (!tinfo.gl.id) {
      m_env->LogDebug(Utility::Format("Failed to load texture %s", id.c_str()));
      return 0;
    }

    m_textures[id].id = tinfo.gl.id;
    m_textures[id].texture_width = tinfo.texture_width;
    m_textures[id].texture_height = tinfo.texture_height;
    m_textures[id].surface_width = tinfo.gl.surface_width;
    m_textures[id].surface_height = tinfo.gl.surface_height;

    m_textures[id].sx = 0;
    m_textures[id].sy = 0;
    m_textures[id].ex = (float)tinfo.texture_width / tinfo.gl.surface_width;
    m_textures[id].ey = (float)tinfo.texture_height / tinfo.gl.surface_height;

    return &m_textures[id];
  }
}

