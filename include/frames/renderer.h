// Renderer core

#ifndef FRAMES_RENDERER
#define FRAMES_RENDERER

#include <queue>

namespace Frames {
  // fake opengl typedefs, used so we don't pull the entire header in
  typedef unsigned int GLuint;
  typedef unsigned short GLushort;
  typedef float GLfloat;

  class Environment;
  class Rect;
  class TextureChunk;

  class Renderer {
  public:
    struct Vertex {
      GLfloat x, y;
      GLfloat t, u;
      GLfloat r, g, b, a;
    };

    Renderer(Environment *env);
    ~Renderer();

    void Begin(int width, int height);
    void End();

    Vertex *Request(int vertices);
    void Return(int mode);  // also renders

    void SetTexture(TextureChunk *tex);

    void PushScissor(const Rect &rect);
    void PopScissor();

  private:
    Environment *m_env; // just for debug functionality

    int m_width;
    int m_height;

    GLuint m_buffer;
    GLuint m_buffer_pos;
    
    GLuint m_last_pos;
    GLuint m_last_vertices;

    GLuint m_elements;

    GLuint m_currentTexture;

    void SetScissor(const Rect &rect);
    std::queue<Rect> m_scissor;
  };
};

#endif

