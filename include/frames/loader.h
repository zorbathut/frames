// Loaders for various 

#ifndef FRAMES_LOADER
#define FRAMES_LOADER

namespace Frames {
  class Environment;
  class Stream;
  class TextureInfo;

  namespace Loader {
    namespace PNG {
      bool Is(Stream *stream);
      TextureInfo Load(Environment *env, Stream *stream);
    }
    namespace JPG {
      bool Is(Stream *stream);
      TextureInfo Load(Environment *env, Stream *stream);
    }
    namespace DDS {
      bool Is(Stream *stream);
      TextureInfo Load(Environment *env, Stream *stream);
    }
  }
};

#endif
