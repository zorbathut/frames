// Renderer core

#ifndef FRAMES_RENDERER_OPENGL
#define FRAMES_RENDERER_OPENGL

#include <stack>
#include <vector>

#include "frames/renderer.h"

#include "frames/configuration.h"

// Define needed for glew to link properly
#define GLEW_STATIC
#include <GL/GLew.h>

namespace Frames {
  namespace Configuration {
    /// Creates a Configuration::Renderer for OpenGL.
    RendererPtr RendererOpengl();
  }
  
  namespace detail {
    class TextureBackingOpengl : public TextureBacking {
    public:
      TextureBackingOpengl(Environment *env, int width, int height, Texture::Format format);
      ~TextureBackingOpengl();

      int GlidGet() const { return m_id; }

      virtual void Write(int sx, int sy, const TexturePtr &tex);

    private:
      GLuint m_id;
    };

    class RendererOpengl : public Renderer {
    public:
      RendererOpengl(Environment *env);
      ~RendererOpengl();

      virtual void Begin(int width, int height);
      virtual void End();

      virtual Vertex *Request(int quads);
      virtual void Return(int quads = -1);  // also renders, count lets you optionally specify the number of quads

      virtual TextureBackingPtr TextureCreate(int width, int height, Texture::Format mode);
      virtual void TextureSet(const TextureBackingPtr &tex);

    private:
      void CreateBuffers(int len);

      GLuint m_vertexShader;
      GLuint m_fragmentShader;

      GLuint m_program;

      GLuint m_uniform_size;
      GLuint m_uniform_sampleMode;
      GLuint m_uniform_sprite;

      GLuint m_attrib_position;
      GLuint m_attrib_tex;
      GLuint m_attrib_color;

      GLuint m_vertices;  // handle of vertex buffer
      int m_verticesQuadcount; // size of vertex buffer, in quads
      int m_verticesQuadpos;  // current write cursor to the vertex buffer, in quads
    
      int m_verticesLastQuadpos; // last write cursor to the vertex buffer, in quads
      int m_verticesLastQuadsize;  // size of last Request, in quads

      GLuint m_indices; // handle of index buffer

      GLuint m_currentTexture;

      virtual void ScissorSet(const Rect &rect);

      GLuint CompileShader(int shaderType, const GLchar *data, const char *readabletype);
    };
  }
}

#endif

