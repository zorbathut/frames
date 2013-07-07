// Texture coordinator

#ifndef FRAME_TEXTURE_MANAGER
#define FRAME_TEXTURE_MANAGER

#include <string>
#include <map>
#include <set>

#include <GL/glew.h>

#include <boost/bimap.hpp>

#include "frame/noncopyable.h"
#include "frame/ptr.h"
#include "frame/rect.h"

namespace Frame {
  class Environment;
  class TextureConfig;

  class TextureBacking : public Refcountable<TextureBacking> {
    friend class Refcountable<TextureBacking>;
  public:

    int GetGLID() const { return m_id; }

    void Allocate(int width, int height, int gltype);

    std::pair<int, int> AllocateSubtexture(int width, int height);

  private:
    TextureBacking(Environment *env);
    ~TextureBacking();

    friend class TextureManager;
    friend class TextureChunk;

    Environment *m_env;

    GLuint m_id;

    int m_surface_width;
    int m_surface_height;

    int m_alloc_next_x;
    int m_alloc_cur_y;
    int m_alloc_next_y;
  };
  typedef Ptr<TextureBacking> TextureBackingPtr;

  class TextureChunk : public Refcountable<TextureChunk> {
    friend class Refcountable<TextureChunk>;
  public:
    int GetWidth() const { return m_texture_width; }
    int GetHeight() const { return m_texture_height; }

    const Rect &GetBounds() { return m_bounds; }

    GLuint GetGLID() const { return m_backing->GetGLID(); }

  private:
    TextureChunk();
    ~TextureChunk();

    friend class TextureManager;

    TextureBackingPtr m_backing;

    int m_texture_width;
    int m_texture_height;

    Rect m_bounds;
  };
  typedef Ptr<TextureChunk> TextureChunkPtr;

  class TextureManager : Noncopyable {
  public:
    TextureManager(Environment *env);
    ~TextureManager();

    // Texture creators
    TextureChunkPtr TextureFromId(const std::string &id);
    TextureChunkPtr TextureFromConfig(const TextureConfig &conf, TextureBackingPtr backing = 0);

    TextureBackingPtr BackingCreate(int width, int height, int modeGL); // we'll have to change this to generalized mode at some point

  private:
    // Allows for accessor function calls
    friend class TextureBacking;
    friend class TextureChunk;

    boost::bimap<std::string, TextureChunk *> m_texture; // not refcounted, the refcounting needs to deallocate

    std::set<TextureBacking *> m_backing; // again, not refcounted

    Environment *m_env;

    void Internal_Shutdown_Backing(TextureBacking *backing);
    void Internal_Shutdown_Chunk(TextureChunk *chunk);
  };
};

#endif
