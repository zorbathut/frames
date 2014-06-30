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

#include "frames/raw.h"

#include "frames/cast.h"
#include "frames/configuration.h"
#include "frames/event_definition.h"
#include "frames/renderer.h"

namespace Frames {
  FRAMES_VERB_DEFINE(Raw::Event::Render, ());

  FRAMES_DEFINE_RTTI(Raw, Frame);
  
  Raw *Raw::Create(Layout *parent, const std::string &name = "") {
    if (!parent) {
      Configuration::Get().LoggerGet()->LogError("Attempted to create Raw with null parent");
      return 0;
    }
    return new Raw(parent, name);
  }

  void Raw::RenderElement(detail::Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // Yeah, this is ugly, but we're not about to rig up an entire new event system for const elements, and it's not like it would help anyway.
    // This particular restriction *has* to be enforced by just telling users not to screw it up.
    const_cast<Raw*>(this)->EventTrigger(Event::Render);
  }

  Raw::Raw(Layout *parent, const std::string &name) :
      Frame(parent, name)
  {  };
  Raw::~Raw() { };
}

