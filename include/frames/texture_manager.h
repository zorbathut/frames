// Texture coordinator

#ifndef FRAMES_TEXTURE_MANAGER
#define FRAMES_TEXTURE_MANAGER

#include <string>
#include <map>
#include <set>

#include <GL/glew.h>

#include "frames/ptr.h"

namespace Frames {
  class Environment;
  class TextureConfig;

  class TextureBacking {
  public:

    int GetGLID() const { return m_id; }

  private:
    TextureBacking();
    ~TextureBacking();

    friend class TextureManager;
    friend class TextureChunk;

    Environment *m_env;

    GLuint m_id;

    int m_surface_width;
    int m_surface_height;

    friend class Ptr<TextureBacking>;
    int m_refs;
    void Ref_Add() { ++m_refs; }
    void Ref_Release() { --m_refs; if (!m_refs) delete this; }
  };
  typedef Ptr<TextureBacking> TextureBackingPtr;

  class TextureChunk {
  public:
    int GetWidth() const { return m_texture_width; }
    int GetHeight() const { return m_texture_height; }

    float GetSX() const { return m_sx; }
    float GetSY() const { return m_sy; }
    float GetEX() const { return m_ex; }
    float GetEY() const { return m_ey; }

    GLuint GetGLID() const { return m_backing->GetGLID(); }

  private:
    TextureChunk();
    ~TextureChunk();

    friend class TextureManager;

    TextureBackingPtr m_backing;

    int m_texture_width;
    int m_texture_height;

    float m_sx, m_sy, m_ex, m_ey;

    friend class Ptr<TextureChunk>;
    int m_refs;
    void Ref_Add() { ++m_refs; }
    void Ref_Release() { --m_refs; if (!m_refs) delete this; }
  };
  typedef Ptr<TextureChunk> TextureChunkPtr;

  class TextureManager {
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

    std::map<std::string, TextureChunk *> m_texture; // not refcounted, the refcounting needs to deallocate
    std::map<TextureChunk *, std::string> m_texture_reverse;

    std::set<TextureBacking *> m_backing; // again, not refcounted

    Environment *m_env;

    void Internal_Shutdown_Backing(TextureBacking *backing);
    void Internal_Shutdown_Chunk(TextureChunk *chunk);
  };
};

#endif
