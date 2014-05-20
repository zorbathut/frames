// Renderer core

#ifndef FRAMES_RENDERER
#define FRAMES_RENDERER

#include <stack>
#include <vector>

#include "frames/color.h"
#include "frames/rect.h"

namespace Frames {
  class Environment;
  struct Rect;

  namespace detail {
    // fake opengl typedefs, used so we don't pull the entire header in
    typedef unsigned int GLuint;
    typedef unsigned short GLushort;
    typedef float GLfloat;

    class TextureBacking;
    class TextureChunk;

    class Renderer {
    public:
      struct Vertex {
        Vector p;
        Vector t;
        Color c;
      };

      Renderer(Environment *env);
      ~Renderer();

      void Begin(int width, int height);
      void End();

      Vertex *Request(int quads);
      void Return(int quads = -1);  // also renders, count lets you optionally specify the number of quads

      void TextureSet();
      void TextureSet(TextureChunk *tex);
      void TextureSet(TextureBacking *tex);

      void ScissorPush(Rect rect);
      void ScissorPop();

      void StatePush();
      void StateClean();
      void StatePop();

      void AlphaPush(float alpha);
      float AlphaGet() const;
      void AlphaPop();
    
      static bool WriteCroppedRect(Vertex *vertex, const Rect &screen, const Color &color, const Rect &bounds); // no fancy lerping
      static bool WriteCroppedTexRect(Vertex *vertex, const Rect &screen, const Rect &tex, const Color &color, const Rect &bounds);  // fancy lerping

    private:
      Environment *m_env; // just for debug functionality

      void CreateBuffers(int len);

      int m_width;
      int m_height;

      GLuint m_vertices;  // handle of vertex buffer
      int m_verticesQuadcount; // size of vertex buffer, in quads
      int m_verticesQuadpos;  // current write cursor to the vertex buffer, in quads
    
      int m_verticesLastQuadpos; // last write cursor to the vertex buffer, in quads
      int m_verticesLastQuadsize;  // size of last Request, in quads

      GLuint m_indices; // handle of index buffer

      void Internal_SetTexture(GLuint tex);
      GLuint m_currentTexture;

      void SetScissor(const Rect &rect);
      std::stack<Rect> m_scissor;

      std::vector<float> m_alpha; // we'll only really allocate it once
    };
  }
}

#endif

