
#include "frame/mask.h"

#include "frame/rect.h"
#include "frame/renderer.h"

#include <GL/GLew.h>

namespace Frame {
  Mask *Mask::CreateBare(Layout *parent) {
    return new Mask(parent);
  }
  Mask *Mask::CreateTagged_imp(const char *filename, int line, Layout *parent) {
    Mask *rv = new Mask(parent);
    rv->SetNameStatic(filename);
    rv->SetNameId(line);
    return rv;
  }

  /*static*/ const char *Mask::GetStaticType() {
    return "Mask";
  }

  bool Mask::TestMouseMasking(int x, int y) {
    return x >= GetLeft() && y >= GetTop() && x < GetRight() && y < GetBottom();
  }

  /*static*/ void Mask::luaF_RegisterFunctions(lua_State *L) {
    Frame::luaF_RegisterFunctions(L);
  }

  void Mask::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

    renderer->ScissorPush(GetBounds());
  }

  void Mask::RenderElementPost(Renderer *renderer) const {
    renderer->ScissorPop();

    Frame::RenderElementPost(renderer);
  }

  Mask::Mask(Layout *parent) :
      Frame(parent)
  {
    SetFullMouseMasking(true);
  };
  Mask::~Mask() { };
}

