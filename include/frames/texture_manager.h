// Texture coordinator

#ifndef FRAMES_TEXTURE_MANAGER
#define FRAMES_TEXTURE_MANAGER

#include <string>
#include <map>
#include <set>

#include "os_gl.h"

#include <boost/bimap.hpp>

#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/rect.h"

namespace Frames {
  class Environment;
  class Texture;
  typedef Ptr<Texture> TexturePtr;

  namespace detail {
    class Renderer;
    class TextureBacking;
    typedef Ptr<TextureBacking> TextureBackingPtr;

    class TextureBacking : public Refcountable<TextureBacking> {
      friend class Refcountable<TextureBacking>;
    public:

      int GlidGet() const { return m_id; }

      int WidthGet() const { return m_surface_width; }
      int HeightGet() const { return m_surface_height; }

      Environment *EnvironmentGet() const { return m_env; }

      void Allocate(int width, int height, int gltype);

      std::pair<int, int> AllocateSubtexture(int width, int height);
      void Write(int sx, int sy, const TexturePtr &tex);

    private:
      TextureBacking(Environment *env);
      ~TextureBacking();

      friend class Environment; // temporary
      friend class TextureManager;

      Environment *m_env;

      GLuint m_id;

      int m_surface_width;
      int m_surface_height;

      int m_alloc_next_x;
      int m_alloc_cur_y;
      int m_alloc_next_y;
    };

    class TextureManager : Noncopyable {
    public:
      TextureManager(Environment *env);
      ~TextureManager();

      TextureBackingPtr BackingCreate(int width, int height, int modeGL); // we'll have to change this to generalized mode at some point

    private:
      // Allows for accessor function calls
      friend class TextureBacking;
      friend class Renderer;

      std::set<TextureBacking *> m_backing; // again, not refcounted

      Environment *m_env;

      void BackingInit(TextureBacking *backing);
      void BackingShutdown(TextureBacking *backing);
    };
  }
}

#endif
