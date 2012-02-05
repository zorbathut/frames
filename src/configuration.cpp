
#include "frames/configuration.h"

#include "frames/environment.h"
#include "frames/loader.h"
#include "frames/stream.h"
#include "frames/texture_manager.h"

#include <cstdio>
#include <cstring>

namespace Frames {
  TextureInfo::TextureInfo() {
     std::memset(this, 0, sizeof(*this));
  }

  void TextureInfo::InfoRaw::Allocate(Environment *m_env, int width, int height, Type newtype) {
    Deallocate(m_env);

    owned = true;
    type = newtype;
    stride = width * GetBPP(newtype);
    data = new unsigned char[width * stride];
  }
  void TextureInfo::InfoRaw::Deallocate(Environment *m_env) {
    delete [] data;
  }

  int TextureInfo::InfoRaw::GetBPP(Type type) {
    if (type == RAW_RGBA) {
      return 4;
    } else if (type == RAW_RGB) {
      return 3;
    } else if (type == RAW_L || type == RAW_A) {
      return 1;
    } else {
      // throw error here?
      return 4;
    }
  }

  Configuration::Configuration() : logger(0), textureFromId(0), streamFromId(0), pathFromId(0), textureFromStream(0) { };

  void Configuration::Logger::LogError(const std::string &log) {
    std::printf("Frames error: %s", log.c_str());
  }
  void Configuration::Logger::LogDebug(const std::string &log) {
    std::printf("Frames debug: %s", log.c_str());
  }

  TextureInfo Configuration::TextureFromId::Create(Environment *env, const std::string &id) {
    std::pair<Stream *, ImageType::T> data = env->GetConfiguration().streamFromId->Create(env, id);

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
    if (typeHint == ImageType::UNKNOWN) {
      if (Loader::PNG::Is(stream))
        typeHint = ImageType::PNG;
      else if (Loader::JPG::Is(stream))
        typeHint = ImageType::JPG;
      /*else if (Loader::DDS::Is(stream))
        typeHint = ImageType::DDS;*/
      else
        return TextureInfo(); // give up
    }

    if (typeHint == ImageType::PNG) {
      return Loader::PNG::Load(env, stream);
    } else if (typeHint == ImageType::JPG) {
      return Loader::JPG::Load(env, stream);
    /*} else if (typeHint == ImageType::DDS) {
      return Loader::DDS::Load(env, stream);*/
    } else {
      return TextureInfo(); // give up
    }
  }
}
