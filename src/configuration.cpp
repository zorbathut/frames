
#include "frames/configuration.h"

#include "frames/environment.h"
#include "frames/stream.h"
#include "frames/texture_manager.h"

#include <cstdio>

namespace Frames {
  Configuration::Configuration() : logger(0), textureFromId(0), streamFromId(0), pathFromId(0), textureFromStream(0) { };

  void Configuration::Logger::LogError(const std::string &log) {
    std::printf("Frames error: %s", log.c_str());
  }
  void Configuration::Logger::LogDebug(const std::string &log) {
    std::printf("Frames debug: %s", log.c_str());
  }

  TextureInfo Configuration::TextureFromId::Create(Environment *env, const std::string &id) {
    std::pair<Stream *, ImageType::T> data = env->GetConfiguration().streamFromId->Create(env, id);

    env->LogDebug(Utility::Format("TFID create %08x", data.first));
    if (data.first) {
      TextureInfo rv = env->GetConfiguration().textureFromStream->Create(env, data.first, data.second);
      delete data.first;
      return rv;
    }

    return TextureInfo();
  }

  std::pair<Stream *, ImageType::T> Configuration::StreamFromId::Create(Environment *env, const std::string &id) {
    std::pair<std::string, ImageType::T> path = env->GetConfiguration().pathFromId->Process(env, id);
    if (!path.first.empty()) {
      return std::make_pair(StreamFile::Create(path.first), path.second);
    }
  }

  std::pair<std::string, ImageType::T> Configuration::PathFromId::Process(Environment *env, const std::string &id) {
    return std::make_pair(id, ImageType::UNKNOWN);
  }

  TextureInfo Configuration::TextureFromStream::Create(Environment *env, Stream *stream, ImageType::T typeHint) {
    glEnable(GL_TEXTURE_2D);

    TextureInfo tex;
    glGenTextures(1, &tex.gl_id);
    glBindTexture(GL_TEXTURE_2D, tex.gl_id);

    env->LogDebug(Utility::Format("Ldebug %d", tex.gl_id));

    std::vector<unsigned long> data(256*256);

    for (int i = 0; i < 256 * 256; ++i) {
      int x = i % 256;
      int y = i / 256;
      x /= 16;
      y /= 16;
      if (x % 2 == y % 2)
        data[i] = 0xffffffff;
      else
        data[i] = 0x00000000;
    }

    tex.surface_width = 256;
    tex.surface_height = 256;
    tex.texture_width = 256;
    tex.texture_height = 256;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return tex;
  }
}
