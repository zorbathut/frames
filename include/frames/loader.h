// Loaders for various 

#ifndef FRAMES_LOADER
#define FRAMES_LOADER

#include "frames/ptr.h"

namespace Frames {
  class Environment;
  class Stream;
  class TextureConfig;

  namespace Loader {
    namespace PNG {
      bool Is(const Ptr<Stream> &stream);
      TextureConfig Load(Environment *env, const Ptr<Stream> &stream);
    }
    namespace JPG {
      bool Is(const Ptr<Stream> &stream);
      TextureConfig Load(Environment *env, const Ptr<Stream> &stream);
    }
  }
}

#endif
