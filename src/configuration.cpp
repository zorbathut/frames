
#include "frames/configuration.h"

#include <cstdio>

namespace Frames {
  Configuration::Configuration() { };

  Configuration::Logger::~Logger() {
  }
  void Configuration::Logger::LogError(const std::string &log) {
    std::printf("Frames error: %s", log.c_str());
  }
  void Configuration::Logger::LogDebug(const std::string &log) {
    std::printf("Frames debug: %s", log.c_str());
  }
}
