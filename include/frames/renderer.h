// Renderer core

#ifndef FRAMES_RENDERER
#define FRAMES_RENDERER

namespace Frames {
  // fake opengl typedefs, used so we don't pull the entire header in
  typedef unsigned int GLuint;
  typedef unsigned short GLushort;
  typedef float GLfloat;

  class Renderer {
  public:
    struct Vertex {
      GLfloat x, y;
      GLfloat t, u;
      GLfloat r, g, b, a;
    };

    Renderer();
    ~Renderer();

    void Begin(int width, int height);
    void End();

    Vertex *Request(int vertices);
    void Return(int mode);  // also renders

  private:
    GLuint m_buffer;
    GLuint m_buffer_pos;
    
    GLuint m_last_pos;
    GLuint m_last_vertices;

    GLuint m_elements;
  };
};

#endif

