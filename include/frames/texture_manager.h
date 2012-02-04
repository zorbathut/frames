// Texture coordinator

#ifndef FRAMES_TEXTURE_MANAGER
#define FRAMES_TEXTURE_MANAGER

#include <string>
#include <map>

#include <GL/glew.h>

namespace Frames {
  class Environment;

  class TextureManager {
  public:
    TextureManager(Environment *env);
    ~TextureManager();

    class Texture {
    public:
      Texture();
      ~Texture();

      GLuint id;

      int texture_width;
      int texture_height;

      int surface_width;
      int surface_height;

      float sx, sy, ex, ey;
    };

    Texture *TextureFromId(const std::string &id);

  private:
    std::map<std::string, Texture> m_textures;

    Environment *m_env;
  };
};

#endif
