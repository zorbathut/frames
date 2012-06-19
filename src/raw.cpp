
#include "frames/raw.h"

#include "frames/event_definition.h"
#include "frames/renderer.h"

#include <GL/GLew.h>

namespace Frames {
  Raw *Raw::CreateBare(Layout *parent) {
    return new Raw(parent);
  }
  Raw *Raw::CreateTagged_imp(const char *filename, int line, Layout *parent) {
    Raw *rv = new Raw(parent);
    rv->SetNameStatic(filename);
    rv->SetNameId(line);
    return rv;
  }

  /*static*/ const char *Raw::GetStaticType() {
    return "Raw";
  }

  /*static*/ void Raw::l_RegisterFunctions(lua_State *L) {
    Frame::l_RegisterFunctions(L);

    FRAMES_FRAMEEVENT_L_REGISTER(L, GetStaticType(), Render, &Layout::l_EventPusher_Default);
  }

  FRAMES_FRAMEEVENT_DEFINE(Raw, Render, (), (EventHandle *handle), (FRAMES_FRAMEEVENT_DEFINE_PARAMETER_PREFIX));

  void Raw::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // We assume that you're not going to be using a RenderElement if you're not planning on attaching rendering to it. TODO: Provide some way to restrict what needs to be cleaned!
    renderer->StatePush();
    renderer->StateClean();

    const_cast<Raw*>(this)->EventRenderTrigger(); // Yeah, this is ugly, but we're not about to rig up an entire new event system for const elements, and it's not like it would help anyway.
    // This particular restriction *has* to be enforced by just telling users not to screw it up.

    renderer->StatePop();
  }

  Raw::Raw(Layout *parent) :
      Frame(parent)
  {  };
  Raw::~Raw() { };
}

