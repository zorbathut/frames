// The Raw class

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
