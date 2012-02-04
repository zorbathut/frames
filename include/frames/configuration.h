// The Configuration class

#ifndef FRAMES_CONFIGURATION
#define FRAMES_CONFIGURATION

#include <string>

#include <GL/GLew.h>

namespace Frames {
  class Environment;
  class Stream;
  class TextureInfo;

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

  // This is what's returned from the configuration to the internal systems
  struct TextureInfo {
    TextureInfo() : gl_id(0), texture_width(0), texture_height(0), surface_width(0), surface_height(0) { };

    GLuint gl_id;

    int texture_width;
    int texture_height;

    int surface_width;
    int surface_height;
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

      virtual TextureInfo Create(Environment *env, const std::string &id);
    };
    TextureFromId *textureFromId;

    class StreamFromId {
    public:
      virtual ~StreamFromId() { }

      virtual std::pair<Stream *, ImageType::T> Create(Environment *env, const std::string &id);
    };
    StreamFromId *streamFromId;

    class PathFromId {
    public:
      virtual ~PathFromId() { }

      virtual std::pair<std::string, ImageType::T> Process(Environment *env, const std::string &id);
    };
    PathFromId *pathFromId;

    class TextureFromStream {
    public:
      virtual ~TextureFromStream() { }

      virtual TextureInfo Create(Environment *env, Stream *stream, ImageType::T typeHint);
    };
    TextureFromStream *textureFromStream;
  };
}

#endif
