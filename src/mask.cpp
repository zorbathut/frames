
#include "frames/mask.h"

#include "frames/rect.h"
#include "frames/renderer.h"

#include "frames/os_gl.h"

namespace Frames {
  Mask *Mask::Create(const std::string &name, Layout *parent) {
    return new Mask(name, parent);
  }

  /*static*/ const char *Mask::GetStaticType() {
    return "Mask";
  }

  bool Mask::TestMouseMasking(float x, float y) {
    return x >= GetLeft() && y >= GetTop() && x < GetRight() && y < GetBottom();
  }

  /*static*/ void Mask::luaF_RegisterFunctions(lua_State *L) {
    Frame::luaF_RegisterFunctions(L);
  }

  void Mask::RenderElement(detail::Renderer *renderer) const {
    Frame::RenderElement(renderer);

    renderer->ScissorPush(GetBounds());
  }

  void Mask::RenderElementPost(detail::Renderer *renderer) const {
    renderer->ScissorPop();

    Frame::RenderElementPost(renderer);
  }

  Mask::Mask(const std::string &name, Layout *parent) :
      Frame(name, parent)
  {
    SetFullMouseMasking(true);
  };
  Mask::~Mask() { };
}

