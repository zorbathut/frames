
#include "frames/configuration.h"

#include "frames/environment.h"
#include "frames/loader.h"
#include "frames/stream.h"
#include "frames/texture_config.h"

#include <cstdio>
#include <cstring>

namespace Frames {
  Configuration::Configuration() : logger(0), textureFromId(0), streamFromId(0), pathFromId(0), textureFromStream(0) { };

  void Configuration::Logger::LogError(const std::string &log) {
    std::printf("Frames error: %s", log.c_str());
  }
  void Configuration::Logger::LogDebug(const std::string &log) {
    std::printf("Frames debug: %s", log.c_str());
  }

  TextureConfig Configuration::TextureFromId::Create(Environment *env, const std::string &id) {
    std::pair<Stream *, ImageType::T> data = env->GetConfiguration().streamFromId->Create(env, id);

    if (data.first) {
      TextureConfig rv = env->GetConfiguration().textureFromStream->Create(env, data.first, data.second);
      delete data.first;
      return rv;
    }

    return TextureConfig();
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

  TextureConfig Configuration::TextureFromStream::Create(Environment *env, Stream *stream, ImageType::T typeHint) {
    if (typeHint == ImageType::UNKNOWN) {
      if (Loader::PNG::Is(stream))
        typeHint = ImageType::PNG;
      else if (Loader::JPG::Is(stream))
        typeHint = ImageType::JPG;
      /*else if (Loader::DDS::Is(stream))
        typeHint = ImageType::DDS;*/
      else
        return TextureConfig(); // give up
    }

    if (typeHint == ImageType::PNG) {
      return Loader::PNG::Load(env, stream);
    } else if (typeHint == ImageType::JPG) {
      return Loader::JPG::Load(env, stream);
    /*} else if (typeHint == ImageType::DDS) {
      return Loader::DDS::Load(env, stream);*/
    } else {
      return TextureConfig(); // give up
    }
  }
}
