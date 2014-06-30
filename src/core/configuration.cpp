/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

#include "frames/configuration.h"

#include "frames/environment.h"
#include "frames/loader.h"
#include "frames/stream.h"
#include "frames/texture.h"

#include <cstdio>
#include <cstring>

#include <windows.h>

namespace Frames {

  void Configuration::Logger::LogError(const std::string &log) {
    std::printf("Frame error: %s", log.c_str());
    MessageBox(0, log.c_str(), "Frame error", MB_OK | MB_ICONEXCLAMATION);
  }
  void Configuration::Logger::LogDebug(const std::string &log) {
    std::printf("Frame debug: %s", log.c_str());
  }

  TexturePtr Configuration::TextureFromId::Create(Environment *env, const std::string &id) {
    StreamPtr stream = env->ConfigurationGet().StreamFromIdGet()->Create(env, id);

    if (stream) {
      return env->ConfigurationGet().TextureFromStreamGet()->Create(env, stream);
    }

    return TexturePtr();
  }

  StreamPtr Configuration::StreamFromId::Create(Environment *env, const std::string &id) {
    std::string path = env->ConfigurationGet().PathFromIdGet()->Process(env, id);
    if (!path.empty()) {
      return StreamFile::Create(path);
    }

    return StreamPtr();
  }

  std::string Configuration::PathFromId::Process(Environment *env, const std::string &id) {
    return id;
  }

  TexturePtr Configuration::TextureFromStream::Create(Environment *env, const StreamPtr &stream) {
    if (Loader::PNG::Is(stream))
      return Loader::PNG::Load(env, stream);
    else if (Loader::JPG::Is(stream))
      return Loader::JPG::Load(env, stream);
    else
      return TexturePtr(); // give up
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

  Configuration::Global MakeDefault() {
    Configuration::Global def;
    def.LoggerSet(Configuration::LoggerPtr(new Configuration::Logger()));
    return def;
  }

  static Configuration::Global s_configGlobal = MakeDefault();

  void Configuration::Set(const Configuration::Global &config) {
    s_configGlobal = config;
    if (!s_configGlobal.LoggerGet()) {
      s_configGlobal.LoggerSet(Configuration::LoggerPtr(new Configuration::Logger()));
    }
  }

  const Configuration::Global &Configuration::Get() {
    return s_configGlobal;
  }
}
