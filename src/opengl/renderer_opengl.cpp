
#include "frames/renderer_opengl.h"

#include "frames/configuration.h"
#include "frames/detail.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
#include "frames/rect.h"

#include "boost/static_assert.hpp"

// Define needed for glew to link properly
#define GLEW_STATIC
#include <GL/GLew.h>

#include <vector>
#include <algorithm>

using namespace std;

BOOST_STATIC_ASSERT(sizeof(Frames::detail::GLfloat) == sizeof(GLfloat));
BOOST_STATIC_ASSERT(sizeof(Frames::detail::GLushort) == sizeof(GLushort));
BOOST_STATIC_ASSERT(sizeof(Frames::detail::GLuint) == sizeof(GLuint));
BOOST_STATIC_ASSERT(sizeof(Frames::Color) == sizeof(GLfloat) * 4);

namespace Frames {
  namespace Configuration {
    class CfgRendererOpengl : public Renderer {
    public:
      virtual detail::Renderer *Create(Environment *env) const {
        return new detail::RendererOpengl(env);
      }
    };

    Configuration::RendererPtr Configuration::RendererOpengl() {
      return Configuration::RendererPtr(new CfgRendererOpengl());
    }
  }

  namespace detail {
    TextureBackingOpengl::TextureBackingOpengl(Environment *env, int width, int height, Texture::Format format) : TextureBacking(env, width, height, format), m_id(0) {
      glGenTextures(1, &m_id);
      if (!m_id) {
        // whoops
        EnvironmentGet()->LogError(detail::Format("Failure to allocate room for texture"));
        return;
      }

      int input_tex_mode;
      if (format == Texture::FORMAT_RGBA_8) {
        input_tex_mode = GL_RGBA;
      } else if (format == Texture::FORMAT_RGB_8) {
        input_tex_mode = GL_RGBA; // we don't use GL_RGB because it's less efficient
      } else if (format == Texture::FORMAT_L_8) {
        input_tex_mode = GL_LUMINANCE;
      } else if (format == Texture::FORMAT_A_8) {
        input_tex_mode = GL_ALPHA;
      } else {
        EnvironmentGet()->LogError(detail::Format("Unrecognized raw type %d in texture", format));
        return;
      }

      // setup standard texture parameters
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_id);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);

      glBindTexture(GL_TEXTURE_2D, m_id);
      glTexImage2D(GL_TEXTURE_2D, 0, input_tex_mode, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); // I'm assuming the last three values are irrelevant
    }

    TextureBackingOpengl::~TextureBackingOpengl() {
      glDeleteTextures(1, &m_id);
    }

    void TextureBackingOpengl::Write(int sx, int sy, const TexturePtr &tex) {
      if (tex->TypeGet() != Texture::RAW) {
        EnvironmentGet()->LogError(detail::Format("Unrecognized type %d in texture", tex->TypeGet()));
        return;
      }

      int input_tex_mode;
      if (tex->FormatGet() == Texture::FORMAT_RGBA_8) {
        input_tex_mode = GL_RGBA;
      } else if (tex->FormatGet() == Texture::FORMAT_RGB_8) {
        input_tex_mode = GL_RGB;
      } else if (tex->FormatGet() == Texture::FORMAT_L_8) {
        input_tex_mode = GL_LUMINANCE;
      } else if (tex->FormatGet() == Texture::FORMAT_A_8) {
        input_tex_mode = GL_ALPHA;
      } else {
        EnvironmentGet()->LogError(detail::Format("Unrecognized format %d in texture", tex->FormatGet()));
        return;
      }

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_id);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if (tex->RawStrideGet() == Texture::RawBPPGet(tex->FormatGet()) * tex->WidthGet()) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, sx, sy, tex->WidthGet(), tex->HeightGet(), input_tex_mode, GL_UNSIGNED_BYTE, tex->RawDataGet());
      } else {
        for (int y = 0; y < tex->HeightGet(); ++y) {
          glTexSubImage2D(GL_TEXTURE_2D, 0, sx, sy + y, tex->WidthGet(), 1, input_tex_mode, GL_UNSIGNED_BYTE, tex->RawDataGet() + y * tex->RawStrideGet());
        }
      }
    }

    RendererOpengl::RendererOpengl(Environment *env) :
        Renderer(env),
        m_vertices(0),
        m_verticesQuadcount(0),
        m_verticesQuadpos(0),
        m_verticesLastQuadsize(0),
        m_verticesLastQuadpos(0),
        m_currentTexture(0)
    {
      // easier to handle it on our own, and we won't be creating environments often enough for this to be a performance hit
      glewInit();

      glGenBuffers(1, &m_vertices);
      glGenBuffers(1, &m_indices);

      CreateBuffers(1 << 16); // maximum size that will fit in a ushort
    };

    RendererOpengl::~RendererOpengl() {
      glDeleteBuffers(1, &m_vertices);
      glDeleteBuffers(1, &m_indices);
    }

    void RendererOpengl::Begin(int width, int height) {
      Renderer::Begin(width, height);

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

      glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);

      glVertexPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, p));
      glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, t));
      glColorPointer(4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, c));

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);
      m_currentTexture = 0;

      glEnable(GL_SCISSOR_TEST);
      ScissorSet(Rect(0, 0, (float)WidthGet(), (float)HeightGet()));
    }

    void RendererOpengl::End() {
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      StatePop();
    }

    Renderer::Vertex *RendererOpengl::Request(int quads) {
      if (quads > m_verticesQuadcount) {
        EnvironmentGet()->LogError("Exceeded valid quad count in a single draw call; splitting NYI");
      }

      if (m_verticesQuadpos + quads > m_verticesQuadcount) {
        // we'll have to clear it out
        glBufferData(GL_ARRAY_BUFFER, m_verticesQuadcount * 4 * sizeof(Vertex), 0, GL_STREAM_DRAW);
        m_verticesQuadpos = 0;
      }

      // now we have acceptable data
      Vertex *rv = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, m_verticesQuadpos * 4 * sizeof(Vertex), quads * 4 * sizeof(Vertex), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);

      m_verticesLastQuadpos = m_verticesQuadpos;
      m_verticesLastQuadsize = quads;
      m_verticesQuadpos += quads;

      return rv;
    }

    void RendererOpengl::Return(int quads /*= -1*/) {
      glUnmapBuffer(GL_ARRAY_BUFFER);

      if (quads == -1) quads = m_verticesLastQuadsize;

      glDrawElements(GL_TRIANGLES, quads * 6, GL_UNSIGNED_SHORT, (void*)(m_verticesLastQuadpos * 6 * sizeof(GLushort)));
    }

    TextureBackingPtr RendererOpengl::TextureCreate(int width, int height, Texture::Format mode) {
      return TextureBackingPtr(new TextureBackingOpengl(EnvironmentGet(), width, height, mode));
    }

    void RendererOpengl::TextureSet(const detail::TextureBackingPtr &tex) {
      int glid = tex.Get() ? static_cast<TextureBackingOpengl*>(tex.Get())->GlidGet() : 0;
      if (m_currentTexture != glid) {
        m_currentTexture = glid;
        glBindTexture(GL_TEXTURE_2D, glid);
      }
    }

    void RendererOpengl::StatePush() {
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
    void RendererOpengl::StateClean() {
      glBindTexture(GL_TEXTURE_2D, 0);

      // we intentionally leave the translation matrices
    }
    void RendererOpengl::StatePop() {
      glPopClientAttrib();
      glPopAttrib();

      glMatrixMode(GL_TEXTURE);
      glPopMatrix();

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
    
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }
  
    void RendererOpengl::ScissorSet(const Rect &rect) {
      glScissor((int)floor(rect.s.x + 0.5f), (int)floor(HeightGet() - rect.e.y + 0.5f), (int)floor(rect.e.x - rect.s.x + 0.5f), (int)floor(rect.e.y - rect.s.y + 0.5f));
    }

    void RendererOpengl::CreateBuffers(int len) {
      int quadLen = len / 4;
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);
      vector<GLushort> elements(quadLen * 6);
      int writepos = 0;
      for (int i = 0; i < quadLen; ++i) {
        elements[writepos++] = i * 4 + 0;
        elements[writepos++] = i * 4 + 1;
        elements[writepos++] = i * 4 + 3;
        elements[writepos++] = i * 4 + 1;
        elements[writepos++] = i * 4 + 2;
        elements[writepos++] = i * 4 + 3;
      }
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLushort), &elements[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      m_verticesQuadcount = quadLen;
      m_verticesQuadpos = m_verticesQuadcount; // will force an array rebuild
    }
  }
}

