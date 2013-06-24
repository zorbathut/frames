// Loaders for various 

#ifndef FRAME_LOADER
#define FRAME_LOADER

namespace Frame {
  class Environment;
  class Stream;
  class TextureConfig;

  namespace Loader {
    namespace PNG {
      bool Is(Stream *stream);
      TextureConfig Load(Environment *env, Stream *stream);
    }
    namespace JPG {
      bool Is(Stream *stream);
      TextureConfig Load(Environment *env, Stream *stream);
    }
    namespace DDS {
      bool Is(Stream *stream);
      TextureConfig Load(Environment *env, Stream *stream);
    }
  }
};

#endif
