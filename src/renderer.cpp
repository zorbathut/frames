
#include "frames/renderer.h"

#include "frames/environment.h"
#include "frames/rect.h"
#include "frames/texture_manager.h"
#include "frames/utility.h"

#include "boost/static_assert.hpp"

#include <GL/glew.h>

#include <vector>
#include <algorithm>

using namespace std;

BOOST_STATIC_ASSERT(sizeof(Frames::GLfloat) == sizeof(GLfloat));
BOOST_STATIC_ASSERT(sizeof(Frames::GLushort) == sizeof(GLushort));
BOOST_STATIC_ASSERT(sizeof(Frames::GLuint) == sizeof(GLuint));
BOOST_STATIC_ASSERT(sizeof(Frames::Color) == sizeof(GLfloat) * 4);

namespace Frames {
  const int bufferElements = 1 << 16; // fit in a ushort
  const int bufferSize = bufferElements * sizeof(Renderer::Vertex);

  Renderer::Renderer(Environment *env) :
      m_env(env),
      m_width(0),
      m_height(0),
      m_buffer(0),
      m_buffer_pos(bufferElements),
      m_last_pos(0),
      m_last_vertices(0),
      m_elements(0),
      m_currentTexture(0)
  {  // set to bufferElements so we create a real buffer when we need it
    glGenBuffers(1, &m_buffer);
    glGenBuffers(1, &m_elements);

    // init the elements buffer - later we should think more about how to make this general-purpose, but for now, we won't really have repeated vertices anyway
    {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elements);
      vector<GLushort> elements(bufferElements);
      for (int i = 0; i < elements.size(); ++i) {
        elements[i] = i;
      }
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLushort), &elements[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
  };

  Renderer::~Renderer() {
    glDeleteBuffers(1, &m_buffer);
    glDeleteBuffers(1, &m_elements);
  }

  void Renderer::Begin(int width, int height) {
    m_width = width;
    m_height = height;

    glPushAttrib(~0);
    glPushClientAttrib(~0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glTranslatef(-1.f, 1.f, 0.f);
    glScalef(2.f / width, -2.f / height, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elements);

    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glColorPointer(4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, c));

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_currentTexture = 0;
  }

  void Renderer::End() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glPopClientAttrib();
    glPopAttrib();
  }

  Renderer::Vertex *Renderer::Request(int vertices) {
    int size = vertices * sizeof(Vertex);
    if (m_buffer_pos + vertices > bufferElements) {
      // we'll have to clear it out
      glBufferData(GL_ARRAY_BUFFER, max(size, bufferSize), 0, GL_STREAM_DRAW);
      m_buffer_pos = 0;
    }

    // now we have acceptable data
    Vertex *rv = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, m_buffer_pos * sizeof(Vertex), size, GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);

    m_last_pos = m_buffer_pos;
    m_last_vertices = vertices;
    m_buffer_pos += vertices;

    return rv;
  }

  void Renderer::Return(int mode, int count /*= -1*/) {
    glUnmapBuffer(GL_ARRAY_BUFFER);

    if (count == -1) count = m_last_vertices;

    glDrawElements(mode, count, GL_UNSIGNED_SHORT, (void*)(m_last_pos * sizeof(GLushort)));
  }

  void Renderer::SetTexture() {
    Internal_SetTexture(0);
  }

  void Renderer::SetTexture(TextureBacking *tex) {
    Internal_SetTexture(tex ? tex->GetGLID() : 0);
  }

  void Renderer::SetTexture(TextureChunk *tex) {
    Internal_SetTexture(tex ? tex->GetGLID() : 0);
  }

  void Renderer::Internal_SetTexture(GLuint tex) {
    if (m_currentTexture != tex) {
      m_currentTexture = tex;
      glBindTexture(GL_TEXTURE_2D, tex);
    }
  }

  void Renderer::PushScissor(const Rect &rect) {
    if (m_scissor.empty()) {
      glEnable(GL_SCISSOR_TEST);
    }

    m_scissor.push(rect);

    SetScissor(rect);
  }

  void Renderer::PopScissor() {
    m_scissor.pop();

    if (m_scissor.empty()) {
      glDisable(GL_SCISSOR_TEST);
    } else {
      SetScissor(m_scissor.back());
    }
  }

  void Renderer::SetScissor(const Rect &rect) {
    glScissor(rect.s.x, m_height - rect.e.y, rect.e.x - rect.s.x, rect.e.y - rect.s.y);
  }
}

