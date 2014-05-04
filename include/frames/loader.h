// Loaders for various 

#ifndef FRAMES_LOADER
#define FRAMES_LOADER

#include "frames/ptr.h"

namespace Frames {
  class Environment;
  class Stream;
  typedef Ptr<Stream> StreamPtr;
  class Texture;
  typedef Ptr<Texture> TexturePtr;

  namespace Loader {
    namespace PNG {
      bool Is(const StreamPtr &stream);
      TexturePtr Load(Environment *env, const StreamPtr &stream);
    }
    namespace JPG {
      bool Is(const StreamPtr &stream);
      TexturePtr Load(Environment *env, const StreamPtr &stream);
    }
  }
}

#endif
