
#include "frames/raw.h"

#include "frames/cast.h"
#include "frames/event_definition.h"
#include "frames/renderer.h"

#include "frames/os_gl.h"

namespace Frames {
  FRAMES_VERB_DEFINE(Raw::Event::Render, ());

  FRAMES_DEFINE_RTTI(Raw, Frame);
  
  Raw *Raw::Create(const std::string &name, Layout *parent) {
    return new Raw(name, parent);
  }

  /*static*/ const char *Raw::GetStaticType() {
    return "Raw";
  }

  /*static*/ void Raw::luaF_RegisterFunctions(lua_State *L) {
    Frame::luaF_RegisterFunctions(L);
  }

  void Raw::RenderElement(detail::Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // We assume that you're not going to be using a RenderElement if you're not planning on attaching rendering to it. TODO: Provide some way to restrict what needs to be cleaned!
    renderer->StatePush();
    renderer->StateClean();

    // TODO: alpha?

    // Yeah, this is ugly, but we're not about to rig up an entire new event system for const elements, and it's not like it would help anyway.
    // This particular restriction *has* to be enforced by just telling users not to screw it up.
    const_cast<Raw*>(this)->EventTrigger(Event::Render);

    renderer->StatePop();
  }

  Raw::Raw(const std::string &name, Layout *parent) :
      Frame(name, parent)
  {  };
  Raw::~Raw() { };
}

