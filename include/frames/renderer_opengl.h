// Renderer core

#ifndef FRAMES_RENDERER_OPENGL
#define FRAMES_RENDERER_OPENGL

#include <stack>
#include <vector>

#include "frames/renderer.h"

namespace Frames {
  namespace detail {
    // fake opengl typedefs, used so we don't pull the entire header in
    typedef unsigned int GLuint;
    typedef unsigned short GLushort;
    typedef float GLfloat;

    class TextureBackingOpengl : public TextureBacking {
    public:
      TextureBackingOpengl(Environment *env) : TextureBacking(env) {};
    };

    class RendererOpengl : public Renderer {
    public:
      RendererOpengl(Environment *env);
      ~RendererOpengl();

      virtual void Begin(int width, int height);
      virtual void End();

      virtual Vertex *Request(int quads);
      virtual void Return(int quads = -1);  // also renders, count lets you optionally specify the number of quads

      virtual TextureBackingPtr TextureCreate();
      virtual void TextureSet(const TextureBackingPtr &tex);

      virtual void StatePush();
      virtual void StateClean();
      virtual void StatePop();

    private:
      void CreateBuffers(int len);

      GLuint m_vertices;  // handle of vertex buffer
      int m_verticesQuadcount; // size of vertex buffer, in quads
      int m_verticesQuadpos;  // current write cursor to the vertex buffer, in quads
    
      int m_verticesLastQuadpos; // last write cursor to the vertex buffer, in quads
      int m_verticesLastQuadsize;  // size of last Request, in quads

      GLuint m_indices; // handle of index buffer

      void Internal_SetTexture(GLuint tex);
      GLuint m_currentTexture;

      virtual void ScissorSet(const Rect &rect);
      std::stack<Rect> m_scissor;
    };
  }
}

#endif

