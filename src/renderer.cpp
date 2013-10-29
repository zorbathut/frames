
#include "frame/renderer.h"

#include "frame/environment.h"
#include "frame/rect.h"
#include "frame/texture_manager.h"
#include "frame/utility.h"

#include "boost/static_assert.hpp"

#include <GL/glew.h>

#include <vector>
#include <algorithm>

using namespace std;

BOOST_STATIC_ASSERT(sizeof(Frame::GLfloat) == sizeof(GLfloat));
BOOST_STATIC_ASSERT(sizeof(Frame::GLushort) == sizeof(GLushort));
BOOST_STATIC_ASSERT(sizeof(Frame::GLuint) == sizeof(GLuint));
BOOST_STATIC_ASSERT(sizeof(Frame::Color) == sizeof(GLfloat) * 4);

namespace Frame {
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
      for (int i = 0; i < (int)elements.size(); ++i) {
        elements[i] = i;
      }
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLushort), &elements[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // prime our alpha stack
    m_alpha.push_back(1);
  };

  Renderer::~Renderer() {
    glDeleteBuffers(1, &m_buffer);
    glDeleteBuffers(1, &m_elements);
  }

  void Renderer::Begin(int width, int height) {
    m_width = width;
    m_height = height;

    StatePush();

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

    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, p));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, t));
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

    StatePop();
  }

  Renderer::Vertex *Renderer::Request(int vertices) {
    int size = vertices * sizeof(Vertex);
    if ((int)m_buffer_pos + vertices > bufferElements) {
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

  void Renderer::ScissorPush(const Rect &rect) {
    if (m_scissor.empty()) {
      glEnable(GL_SCISSOR_TEST);
    }

    m_scissor.push(rect);

    SetScissor(rect);
  }

  void Renderer::ScissorPop() {
    m_scissor.pop();

    if (m_scissor.empty()) {
      glDisable(GL_SCISSOR_TEST);
    } else {
      SetScissor(m_scissor.back());
    }
  }

  void Renderer::StatePush() {
    // THIS IS DEFINITELY NOT HORRIFYINGLY SLOW
    glPushAttrib(~0);
    glPushClientAttrib(~0);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
  }
  void Renderer::StateClean() {
    glBindTexture(GL_TEXTURE_2D, 0);

    // we intentionally leave the translation matrices
  }
  void Renderer::StatePop() {
    glPopClientAttrib();
    glPopAttrib();

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }
  
  void Renderer::AlphaPush(float alpha) {
    m_alpha.push_back(alpha * AlphaGet());
  }

  float Renderer::AlphaGet() const {
    return m_alpha.back();
  }

  void Renderer::AlphaPop() {
    m_alpha.pop_back();
  }

  bool Renderer::WriteCroppedRect(Vertex *verts, const Rect &screen, const Color &color, const Rect &bounds) {
    if (screen.s.x > bounds.e.x || screen.e.x < bounds.s.x || screen.s.y > bounds.e.y || screen.e.y < bounds.s.y) {
      return false;
    }

    // set up boundaries
    verts[0].p = Utility::Clamp(screen.s, bounds.s, bounds.e);
    verts[2].p = Utility::Clamp(screen.e, bounds.s, bounds.e);

    // spread it out
    verts[1].p.x = verts[2].p.x;
    verts[1].p.y = verts[0].p.y;
    verts[3].p.x = verts[0].p.x;
    verts[3].p.y = verts[2].p.y;

    // colorize
    verts[0].c = color;
    verts[1].c = color;
    verts[2].c = color;
    verts[3].c = color;

    return true;
  }

  bool Renderer::WriteCroppedTexRect(Vertex *vertex, const Rect &screen, const Rect &tex, const Color &color, const Rect &bounds) {
    if (screen.s.x > bounds.e.x || screen.e.x < bounds.s.x || screen.s.y > bounds.e.y || screen.e.y < bounds.s.y) {
      return false;
    }
    
    // generate [0] and [2], derive the others from that
    if (screen.s.x >= bounds.s.x && screen.e.x <= bounds.e.x && screen.s.y >= bounds.s.y && screen.e.y <= bounds.e.y) {
      // easy solution, we're fully within bounds
      vertex[0].p = screen.s;
      vertex[0].t = tex.s;
    
      vertex[2].p = screen.e;
      vertex[2].t = tex.e;
    } else {
      // we intersect, so we need to do all the work
      vertex[0].p.x = std::max(screen.s.x, bounds.s.x);
      vertex[0].p.y = std::max(screen.s.y, bounds.s.y);
      
      vertex[2].p.x = std::min(screen.e.x, bounds.e.x);
      vertex[2].p.y = std::min(screen.e.y, bounds.e.y);
      
      float xs = (tex.e.x - tex.s.x) / (screen.e.x - screen.s.x);
      float ys = (tex.e.y - tex.s.y) / (screen.e.y - screen.s.y);
      
      vertex[0].t.x = (vertex[0].p.x - screen.s.x) * xs + tex.s.x;
      vertex[0].t.y = (vertex[0].p.y - screen.s.y) * ys + tex.s.y;
      
      vertex[2].t.x = (vertex[2].p.x - screen.s.x) * xs + tex.s.x;
      vertex[2].t.y = (vertex[2].p.y - screen.s.y) * ys + tex.s.y;
    }
    
    // now we've got a valid [0] and [2], make textures
    vertex[1].p.x = vertex[2].p.x;
    vertex[1].p.y = vertex[0].p.y;
    
    vertex[3].p.x = vertex[0].p.x;
    vertex[3].p.y = vertex[2].p.y;
    
    vertex[1].t.x = vertex[2].t.x;
    vertex[1].t.y = vertex[0].t.y;
    
    vertex[3].t.x = vertex[0].t.x;
    vertex[3].t.y = vertex[2].t.y;
    
    // copy all colors over
    vertex[0].c = color;
    vertex[1].c = color;
    vertex[2].c = color;
    vertex[3].c = color;
    
    return true;
  }

  void Renderer::SetScissor(const Rect &rect) {
    glScissor((int)floor(rect.s.x + 0.5f), (int)floor(m_height - rect.e.y + 0.5f), (int)floor(rect.e.x - rect.s.x + 0.5f), (int)floor(rect.e.y - rect.s.y + 0.5f));
  }
}

