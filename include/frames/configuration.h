// The Configuration class

#ifndef FRAMES_CONFIGURATION
#define FRAMES_CONFIGURATION

#include <string>

#include <GL/GLew.h>

namespace Frames {
  class Environment;
  class Stream;
  class TextureConfig;

  namespace ImageType {
    enum T {
      UNKNOWN,
      DDS,
      PNG,
      JPG,
      // TODO:
      // BMP,
      // TGA,

      USER_BEGIN = 0x1000, // if you need to make your own constants, make them after this value

      INVALID=0xffffffff
    };
  };

  struct Configuration {
  public:
    Configuration(); // init to default

    class Logger {
    public:
      virtual ~Logger() { }

      virtual void LogError(const std::string &log);
      virtual void LogDebug(const std::string &log);
    };
    Logger *logger;

    class TextureFromId {
    public:
      virtual ~TextureFromId() { }

      virtual TextureConfig Create(Environment *env, const std::string &id);
    };
    TextureFromId *textureFromId;

    class StreamFromId {
    public:
      virtual ~StreamFromId() { }

      virtual Stream *Create(Environment *env, const std::string &id);
    };
    StreamFromId *streamFromId;

    class PathFromId {
    public:
      virtual ~PathFromId() { }

      virtual std::string Process(Environment *env, const std::string &id);
    };
    PathFromId *pathFromId;

    class TextureFromStream {
    public:
      virtual ~TextureFromStream() { }

      virtual TextureConfig Create(Environment *env, Stream *stream);
    };
    TextureFromStream *textureFromStream;

    std::string fontDefaultId;
  };
}

#endif
