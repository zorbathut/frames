// The Configuration class

#ifndef FRAMES_CONFIGURATION
#define FRAMES_CONFIGURATION

#include <string>

namespace Frames {
  struct Configuration {
  public:
    Configuration(); // init to default

    class Logger {
    public:
      virtual ~Logger();

      virtual void LogError(const std::string &log);
      virtual void LogDebug(const std::string &log);
    };
    Logger *logger;
  };
}

#endif
