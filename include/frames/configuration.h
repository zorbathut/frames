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
    TextureInfo();

    enum { GL, RAW };
    int mode;

    enum Type {
      RAW_RGBA, // 8bpc, 32bpp, laid out as RGBA
      RAW_RGB, // 8bpc, 24bpp, laid out as RGB. Packed. Will be converted to RGBA for actual textures. Probably slower than RAW_RGBA if conversion happens often.
      RAW_L, // 8bpc, 8bpp, laid out as L.
      RAW_A, // 8bpc, 8bpp, laid out as A.
    }; 

    struct InfoGL {
      GLuint id;

      int surface_width;
      int surface_height;
    };
    struct InfoRaw {

      void Allocate(Environment *m_env, int width, int height, Type type);
      void Deallocate(Environment *m_env);

      static int GetBPP(Type type);

      unsigned char *data;
      bool owned;
      int stride;
      Type type;
    };

    union {
      InfoGL gl;
      InfoRaw raw;
    };

    int texture_width;
    int texture_height;
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
