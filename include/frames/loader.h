// Loaders for various 

#ifndef FRAMES_LOADER
#define FRAMES_LOADER

#include "frames/ptr.h"

namespace Frames {
  class Environment;
  class Stream;
  class Texture;

  namespace Loader {
    namespace PNG {
      bool Is(const Ptr<Stream> &stream);
      Ptr<Texture> Load(Environment *env, const Ptr<Stream> &stream);
    }
    namespace JPG {
      bool Is(const Ptr<Stream> &stream);
      Ptr<Texture> Load(Environment *env, const Ptr<Stream> &stream);
    }
  }
}

#endif
