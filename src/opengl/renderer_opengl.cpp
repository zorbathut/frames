
#include "frames/renderer_opengl.h"

#include "frames/configuration.h"
#include "frames/detail.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
#include "frames/rect.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <algorithm>

// Define needed for glew to link properly
#define GLEW_STATIC
#include <GL/GLew.h>

using namespace std;

BOOST_STATIC_ASSERT(sizeof(Frames::Vector) == sizeof(GLfloat) * 2);
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
    static const GLchar sVertexShader[] =
      "#version 110\n"
      "\n"
      "uniform vec2 size;\n"  // size of screen; changes rarely
      "attribute vec2 position;\n"
      "attribute vec2 tex;\n"
      "attribute vec4 color;\n"
      "\n"
      "varying vec2 pTex;\n"
      "varying vec4 pColor;\n"
      "\n"
      "void main() { vec2 cp = position; cp.x /= size.x; cp.y /= -size.y; cp *= 2.; cp += vec2(-1., 1.); gl_Position = vec4(cp, 0., 1.); pTex = tex; pColor = color; }\n";

    static const GLchar sFragmentShader[] =
      "#version 110\n"
      "\n"
      "varying vec2 pTex;\n"
      "varying vec4 pColor;\n"
      "\n"
      "uniform int sampleMode;\n"  // 0 means don't sample. 1 means do sample and multiply. 2 means do sample and multiply; pretend .rgb is 1.f.
      "uniform sampler2D sprite;\n"  // sprite texture to reference if it is being referenced
      "\n"
      "void main() { vec4 color = pColor; if (sampleMode == 1) color *= texture2D(sprite, pTex); if (sampleMode == 2) color.a *= texture2D(sprite, pTex).r; gl_FragColor = color; }\n";

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
      } else if (format == Texture::FORMAT_R_8) {
        input_tex_mode = GL_RED;
      } else {
        EnvironmentGet()->LogError(detail::Format("Unrecognized raw type %d in texture", format));
        return;
      }

      // setup standard texture parameters
      glBindTexture(GL_TEXTURE_2D, m_id);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glBindTexture(GL_TEXTURE_2D, m_id);
      glTexImage2D(GL_TEXTURE_2D, 0, input_tex_mode, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0); // I'm assuming the last three values are irrelevant
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
      } else if (tex->FormatGet() == Texture::FORMAT_R_8) {
        input_tex_mode = GL_RED;
      } else {
        EnvironmentGet()->LogError(detail::Format("Unrecognized format %d in texture", tex->FormatGet()));
        return;
      }

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
        m_vertexShader(0),
        m_fragmentShader(0),
        m_program(0),
        m_uniform_size(0),
        m_uniform_sampleMode(0),
        m_uniform_sprite(0),
        m_attrib_position(0),
        m_attrib_tex(0),
        m_attrib_color(0),
        m_vao(0),
        m_vertices(0),
        m_verticesQuadcount(0),
        m_verticesQuadpos(0),
        m_verticesLastQuadsize(0),
        m_verticesLastQuadpos(0),
        m_currentTexture(0)
    {
      // easier to handle it on our own, and we won't be creating environments often enough for this to be a performance hit
      glewExperimental = true;  // necessary to work on core profile
      glewInit();

      m_vertexShader = CompileShader(GL_VERTEX_SHADER, sVertexShader, "vertex");
      m_fragmentShader = CompileShader(GL_FRAGMENT_SHADER, sFragmentShader, "fragment");
      
      {
        m_program = glCreateProgram();
        glAttachShader(m_program, m_vertexShader);
        glAttachShader(m_program, m_fragmentShader);
        glLinkProgram(m_program);

        GLint result;
        glGetProgramiv(m_program, GL_LINK_STATUS, &result);
        if (!result) {
          GLint len = 0;
          glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &len);
          std::vector<GLchar> log;
          log.resize(len);
          glGetProgramInfoLog(m_program, len, 0, &log[0]);
          EnvironmentGet()->LogError(detail::Format("Failure to link program: %s", &log[0]));
        }
      }

      m_uniform_size = glGetUniformLocation(m_program, "size");
      m_uniform_sampleMode = glGetUniformLocation(m_program, "sampleMode");
      m_uniform_sprite = glGetUniformLocation(m_program, "sprite");

      m_attrib_position = glGetAttribLocation(m_program, "position");
      m_attrib_tex = glGetAttribLocation(m_program, "tex");
      m_attrib_color = glGetAttribLocation(m_program, "color");
      
      glGenBuffers(1, &m_vertices);
      glGenBuffers(1, &m_indices);

      CreateBuffers(1 << 16); // maximum size that will fit in a ushort

      glGenVertexArrays(1, &m_vao);
      glBindVertexArray(m_vao);

      glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);

      glVertexAttribPointer(m_attrib_position, 2, GL_FLOAT, true, sizeof(Vertex), (void*)offsetof(Vertex, p));
      glVertexAttribPointer(m_attrib_tex, 2, GL_FLOAT, true, sizeof(Vertex), (void*)offsetof(Vertex, t));
      glVertexAttribPointer(m_attrib_color, 4, GL_FLOAT, true, sizeof(Vertex), (void*)offsetof(Vertex, c));

      glEnableVertexAttribArray(m_attrib_position);
      glEnableVertexAttribArray(m_attrib_tex);
      glEnableVertexAttribArray(m_attrib_color);
    };

    RendererOpengl::~RendererOpengl() {
      glDeleteShader(m_vertexShader);
      glDeleteShader(m_fragmentShader);

      glDeleteProgram(m_program);

      glDeleteVertexArrays(1, &m_vao);

      glDeleteBuffers(1, &m_vertices);
      glDeleteBuffers(1, &m_indices);
    }

    void RendererOpengl::Begin(int width, int height) {
      Renderer::Begin(width, height);

      // set up things we actually care about

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBlendEquation(GL_FUNC_ADD);

      glUseProgram(m_program);
      glBindVertexArray(m_vao);

      // I feel like these shouldn't be necessary, but apparently they are?
      glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, 0);
      m_currentTexture = 0;

      glEnable(GL_SCISSOR_TEST);

      glUniform1i(m_uniform_sprite, 0);
      glUniform2f(m_uniform_size, (float)width, (float)height);
      glUniform1i(m_uniform_sampleMode, 0);
    }

    void RendererOpengl::End() {
      glBindVertexArray(0);
      glUseProgram(0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
      TextureBackingOpengl *backing = tex.Get() ? static_cast<TextureBackingOpengl*>(tex.Get()) : 0;
      int glid = backing ? backing->GlidGet() : 0;
      if (m_currentTexture != glid) {
        m_currentTexture = glid;
        glBindTexture(GL_TEXTURE_2D, glid);

        if (m_currentTexture) {
          if (backing->FormatGet() == Texture::FORMAT_R_8) {
            glUniform1i(m_uniform_sampleMode, 2);
          } else {
            glUniform1i(m_uniform_sampleMode, 1);
          }
        } else {
          glUniform1i(m_uniform_sampleMode, 0);
        }
      }
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

    GLuint RendererOpengl::CompileShader(int shaderType, const GLchar *data, const char *readabletype) {
      GLuint rv = glCreateShader(shaderType);
      glShaderSource(rv, 1, &data, 0);
      glCompileShader(rv);
      GLint result;
      glGetShaderiv(rv, GL_COMPILE_STATUS, &result);
      if (!result) {
        GLint len = 0;
        glGetShaderiv(rv, GL_INFO_LOG_LENGTH, &len);
        std::vector<GLchar> log;
        log.resize(len);
        glGetShaderInfoLog(rv, len, 0, &log[0]);
        EnvironmentGet()->LogError(detail::Format("Failure to compile %s shader: %s", readabletype, &log[0]));
      }
      return rv;
    }
  }
}

