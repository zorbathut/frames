
#include "frames/mask.h"

#include "frames/cast.h"
#include "frames/rect.h"
#include "frames/renderer.h"

#include "frames/os_gl.h"

namespace Frames {
  FRAMES_DEFINE_RTTI(Mask, Frame);

  Mask *Mask::Create(const std::string &name, Layout *parent) {
    return new Mask(name, parent);
  }

  /*static*/ const char *Mask::TypeStaticGet() {
    return "Mask";
  }

  bool Mask::MouseMaskingTest(float x, float y) {
    return x >= LeftGet() && y >= TopGet() && x < RightGet() && y < BottomGet();
  }

  /*static*/ void Mask::luaF_RegisterFunctions(lua_State *L) {
    Frame::luaF_RegisterFunctions(L);
  }

  void Mask::RenderElementPreChild(detail::Renderer *renderer) const {
    Frame::RenderElementPreChild(renderer);

    renderer->ScissorPush(BoundsGet());
  }

  void Mask::RenderElementPostChild(detail::Renderer *renderer) const {
    renderer->ScissorPop();

    Frame::RenderElementPostChild(renderer);
  }

  Mask::Mask(const std::string &name, Layout *parent) :
      Frame(name, parent)
  {
    MouseMaskingFullSet(true);
  };
  Mask::~Mask() { };
}

