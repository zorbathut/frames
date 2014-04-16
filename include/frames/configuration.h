// The Configuration class

#ifndef FRAMES_CONFIGURATION
#define FRAMES_CONFIGURATION

#include <string>

#include "os_gl.h"
#include "ptr.h"

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

    class Logger : public Refcountable<Logger> {
    public:
      virtual ~Logger() { }

      virtual void LogError(const std::string &log);
      virtual void LogDebug(const std::string &log);
    };
    Ptr<Logger> logger;

    class TextureFromId : public Refcountable<TextureFromId> {
    public:
      virtual ~TextureFromId() { }

      virtual TextureConfig Create(Environment *env, const std::string &id);
    };
    Ptr<TextureFromId> textureFromId;

    class StreamFromId : public Refcountable<StreamFromId> {
    public:
      virtual ~StreamFromId() { }

      virtual Stream *Create(Environment *env, const std::string &id);
    };
    Ptr<StreamFromId> streamFromId;

    class PathFromId : public Refcountable<PathFromId> {
    public:
      virtual ~PathFromId() { }

      virtual std::string Process(Environment *env, const std::string &id);
    };
    Ptr<PathFromId> pathFromId;

    class TextureFromStream : public Refcountable<TextureFromStream> {
    public:
      virtual ~TextureFromStream() { }

      virtual TextureConfig Create(Environment *env, Stream *stream);
    };
    Ptr<TextureFromStream> textureFromStream;

    std::string fontDefaultId;

    class Clipboard : public Refcountable<Clipboard> {
    public:
      virtual ~Clipboard() { }

      virtual void Set(const std::string &dat);
      virtual std::string Get();
    };
    Ptr<Clipboard> clipboard;

    class Performance : public Refcountable<Performance> {
    public:
      virtual ~Performance() { }

      virtual void *Push(float r, float g, float b) = 0;  // need to rework the performance system, right now it expects floating-point values
      virtual void Pop(void *) = 0;
    };
    Ptr<Performance> performance;
  };
}

#endif
