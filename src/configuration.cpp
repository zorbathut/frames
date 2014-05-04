
#include "frames/configuration.h"

#include "frames/environment.h"
#include "frames/loader.h"
#include "frames/stream.h"
#include "frames/texture_config.h"

#include <cstdio>
#include <cstring>

#include <windows.h>

namespace Frames {
  Configuration::Configuration() { };

  void Configuration::Logger::LogError(const std::string &log) {
    std::printf("Frame error: %s", log.c_str());
    MessageBox(0, log.c_str(), "Frame error", MB_OK | MB_ICONEXCLAMATION);
  }
  void Configuration::Logger::LogDebug(const std::string &log) {
    std::printf("Frame debug: %s", log.c_str());
  }

  TextureConfig Configuration::TextureFromId::Create(Environment *env, const std::string &id) {
    Stream *stream = env->ConfigurationGet().StreamFromIdGet()->Create(env, id);

    if (stream) {
      TextureConfig rv = env->ConfigurationGet().TextureFromStreamGet()->Create(env, stream);
      delete stream;
      return rv;
    }

    return TextureConfig();
  }

  Stream *Configuration::StreamFromId::Create(Environment *env, const std::string &id) {
    std::string path = env->ConfigurationGet().PathFromIdGet()->Process(env, id);
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
    else
      return TextureConfig(); // give up
  }

  // win32 only. TODO: split into crossplatform
  void Configuration::Clipboard::Set(const std::string &dat) {
    // TODO: unicode
    if (OpenClipboard(0)) {
      EmptyClipboard();

      std::string clipboarded;
      for (int i = 0; i < (int)dat.size(); ++i) {
        if (dat[i] == '\n') {
          clipboarded += '\r';
        }
        clipboarded += dat[i];
      }

      HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE, clipboarded.size() + 1);

      if (data) {
        char *write = (char*)GlobalLock(data);
        memcpy(write, clipboarded.c_str(), clipboarded.size() + 1);
        GlobalUnlock(data);

        SetClipboardData(CF_TEXT, data);
      }

      CloseClipboard();
    }
  }
  std::string Configuration::Clipboard::Get() {
    // TODO: unicode
    std::string result;
    if (OpenClipboard(0)) {
      HGLOBAL data = GetClipboardData(CF_TEXT);

      if (data) {
        const char *read = (const char*)GlobalLock(data);
        if (read) {
          while (*read) {
            if (*read != '\r') {
              result += *(read++);
            }
          }
          GlobalUnlock(data);
        }
      }

      CloseClipboard();
    }
    return result;
  }
}
