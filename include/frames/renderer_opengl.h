/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef FRAMES_RENDERER_OPENGL
#define FRAMES_RENDERER_OPENGL

#include <stack>
#include <vector>

#include "frames/renderer.h"

#include "frames/configuration.h"

typedef unsigned int GLuint;
typedef char GLchar;

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

      virtual void Write(int sx, int sy, const TexturePtr &tex) FRAMES_OVERRIDE;

    private:
      GLuint m_id;
    };

    class RendererOpengl : public Renderer {
    public:
      RendererOpengl(Environment *env);
      ~RendererOpengl();

      virtual void Begin(int width, int height) FRAMES_OVERRIDE;
      virtual void End() FRAMES_OVERRIDE;

      virtual Vertex *Request(int quads) FRAMES_OVERRIDE;
      virtual void Return(int quads = -1) FRAMES_OVERRIDE;  // also renders, count lets you optionally specify the number of quads

      virtual TextureBackingPtr TextureCreate(int width, int height, Texture::Format mode) FRAMES_OVERRIDE;
      virtual void TextureSet(const TextureBackingPtr &tex) FRAMES_OVERRIDE;

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

      GLuint m_vao;

      GLuint m_vertices;  // handle of vertex buffer
      int m_verticesQuadcount; // size of vertex buffer, in quads
      int m_verticesQuadpos;  // current write cursor to the vertex buffer, in quads
    
      int m_verticesLastQuadpos; // last write cursor to the vertex buffer, in quads
      int m_verticesLastQuadsize;  // size of last Request, in quads

      GLuint m_indices; // handle of index buffer

      GLuint m_currentTexture;

      virtual void ScissorSet(const Rect &rect) FRAMES_OVERRIDE;

      GLuint CompileShader(int shaderType, const GLchar *data, const char *readabletype);
    };
  }
}

#endif

