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

#include "frames/mask.h"

#include "frames/cast.h"
#include "frames/configuration.h"
#include "frames/rect.h"
#include "frames/renderer.h"

namespace Frames {
  FRAMES_DEFINE_RTTI(Mask, Frame);

  Mask *Mask::Create(Layout *parent, const std::string &name = "") {
    if (!parent) {
      Configuration::Get().LoggerGet()->LogError("Attempted to create Mask with null parent");
      return 0;
    }
    return new Mask(parent, name);
  }

  bool Mask::MouseMaskingTest(float x, float y) const {
    return x >= LeftGet() && y >= TopGet() && x < RightGet() && y < BottomGet();
  }

  void Mask::RenderElementPreChild(detail::Renderer *renderer) const {
    Frame::RenderElementPreChild(renderer);

    renderer->ScissorPush(BoundsGet());
  }

  void Mask::RenderElementPostChild(detail::Renderer *renderer) const {
    renderer->ScissorPop();

    Frame::RenderElementPostChild(renderer);
  }

  Mask::Mask(Layout *parent, const std::string &name) :
      Frame(parent, name)
  {
    MouseMaskingFullSet(true);
  };
  Mask::~Mask() { };
}

