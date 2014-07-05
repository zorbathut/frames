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

#ifndef FRAMES_TEXTURE
#define FRAMES_TEXTURE

#include "frames/frame.h"
#include "frames/texture_chunk.h"

namespace Frames {
  /// Used to render a texture.
  /** The texture will be resized to fill the Sprite, possibly distorting its aspect ratio. */
  class Sprite : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    /// Creates a new Raw.
    static Sprite *Create(Layout *parent, const std::string &name);

    /// Sets the texture by ID.
    /** See \ref basicsresources "Resources" for details on Frames's resource system.
    
    The Sprite's default size will be set to the native size of the texture, in pixels.*/
    void TextureSet(const std::string &id);
    /// Gets the current texture ID.
    const std::string &TextureGet() const { return m_texture_id; }

    // Experimental, disabled for documentation
    /// @cond EXPERIMENTAL
    void EXPERIMENTAL_RotateSet(float angle) { m_angle = angle; }
    float EXPERIMENTAL_RotateGet() const { return m_angle; }

    void EXPERIMENTAL_TintSet(Color color);
    Color EXPERIMENTAL_TintGet() const { return m_tint; }
    /// @endcond

  protected:
    /// Creates a new Sprite with the given parameters. "parent" must be non-null.
    Sprite(Layout *parent, const std::string &name);
    virtual ~Sprite();

    /// Renders the Text.
    virtual void RenderElement(detail::Renderer *renderer) const;

  private:
    std::string m_texture_id;
    detail::TextureChunkPtr m_texture;

    float m_angle;

    Color m_tint;
  };
}

#endif
