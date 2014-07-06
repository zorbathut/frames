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

#ifndef FRAMES_MASK
#define FRAMES_MASK

#include "frames/frame.h"

namespace Frames {
  /// Used for clipping the render area of frames against an arbitrary rectangle.
  /** All children of Mask will be clipped to the dimensions of Mask.
  This is useful for situations where part of a frame may be visible, but the invisible part is not occluded by a frame "above" it.
  A good example is a scrollable list - the body of the list would be contained in a Mask frame.
  
  Mask will also clip mouse input, ensuring that the user can't accidentally click on a clipped UI element.
  
  Mask has no extra public API - its functionality is a property of the type itself.*/
  class Mask : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    /// Creates a new Mask.
    static Mask *Create(Layout *parent, const std::string &name);

  protected:
    /// Creates a new Mask with the given parameters. "parent" must be non-null.
    Mask(Layout *parent, const std::string &name);
    virtual ~Mask();

  private:
    virtual bool MouseMaskingTest(float x, float y);

    virtual void RenderElementPreChild(detail::Renderer *renderer) const;
    virtual void RenderElementPostChild(detail::Renderer *renderer) const;
  };
}

#endif
