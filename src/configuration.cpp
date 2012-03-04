
#include "frames/configuration.h"

#include "frames/environment.h"
#include "frames/loader.h"
#include "frames/stream.h"
#include "frames/texture_config.h"

#include <cstdio>
#include <cstring>

#include <windows.h>

namespace Frames {
  Configuration::Configuration() : logger(0), textureFromId(0), streamFromId(0), pathFromId(0), textureFromStream(0) { };

  void Configuration::Logger::LogError(const std::string &log) {
    std::printf("Frames error: %s", log.c_str());
    MessageBox(0, log.c_str(), "Frames error", MB_OK | MB_ICONEXCLAMATION);
  }
  void Configuration::Logger::LogDebug(const std::string &log) {
    std::printf("Frames debug: %s", log.c_str());
  }

  TextureConfig Configuration::TextureFromId::Create(Environment *env, const std::string &id) {
    Stream *stream = env->GetConfiguration().streamFromId->Create(env, id);

    if (stream) {
      TextureConfig rv = env->GetConfiguration().textureFromStream->Create(env, stream);
      delete stream;
      return rv;
    }

    return TextureConfig();
  }

  Stream *Configuration::StreamFromId::Create(Environment *env, const std::string &id) {
    std::string path = env->GetConfiguration().pathFromId->Process(env, id);
    if (!path.empty()) {
      return StreamFile::Create(path);
    }

    return 0;
  }

  std::string Configuration::PathFromId::Process(Environment *env, const std::string &id) {
    return id;
  }

  TextureConfig Configuration::TextureFromStream::Create(Environment *env, Stream *stream) {
    if (Loader::PNG::Is(stream))
      return Loader::PNG::Load(env, stream);
    else if (Loader::JPG::Is(stream))
      return Loader::JPG::Load(env, stream);
    /*else if (Loader::DDS::Is(stream))
      typeHint = ImageType::DDS;*/
    else
      return TextureConfig(); // give up
  }
}
