
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

