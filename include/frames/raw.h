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

#ifndef FRAMES_RAW
#define FRAMES_RAW

#include "frames/frame.h"

namespace Frames {
  /// Used for deferring to user code for rendering.
  /** Raw is used for injecting arbitrary draw calls in the middle of the standard Frames rendering. When this frame would be rendered, the Render event handlers will be called.
  
  At the moment, there's no offical documentation on what parts of the rendering state need to be returned to default values. This will be provided later - for now, consider this to be an interface in flux. */
  class Raw : public Frame {
  public:
    FRAMES_VERB_DECLARE_BEGIN
      /// Signals that this Raw frame is ready to render.
      /** WARNING: Unlike most verbs, while this verb is being signaled, it is undefined behavior to call *any* non-Get function provided by Frames and associated with this Environment.*/
      FRAMES_VERB_DECLARE(Render, ());
    FRAMES_VERB_DECLARE_END

  private:
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    /// Creates a new Raw.
    static Raw *Create(Layout *parent, const std::string &name);
   
  protected:
    /// Creates a new Raw with the given parameters. "parent" must be non-null.
    Raw(Layout *parent, const std::string &name);
    virtual ~Raw();

    /// Renders the Raw and fires the appropriate event.
    virtual void RenderElement(detail::Renderer *renderer) const;
  };
}

#endif
