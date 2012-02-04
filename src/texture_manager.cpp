
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
    if (!tinfo.gl_id) {
      m_env->LogDebug(Utility::Format("Failed to load texture %s", id.c_str()));
      return 0;
    }

    m_textures[id].id = tinfo.gl_id;
    m_textures[id].texture_width = tinfo.texture_width;
    m_textures[id].texture_height = tinfo.texture_height;
    m_textures[id].surface_width = tinfo.surface_width;
    m_textures[id].surface_height = tinfo.surface_height;

    m_textures[id].sx = 0;
    m_textures[id].sy = 0;
    m_textures[id].ex = (float)tinfo.texture_width / tinfo.surface_width;
    m_textures[id].ey = (float)tinfo.texture_height / tinfo.surface_height;

    return &m_textures[id];
  }
}

