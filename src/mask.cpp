
#include "frames/mask.h"

#include "frames/cast.h"
#include "frames/configuration.h"
#include "frames/rect.h"
#include "frames/renderer.h"

#include "frames/os_gl.h"

namespace Frames {
  FRAMES_DEFINE_RTTI(Mask, Frame);

  Mask *Mask::Create(Layout *parent, const std::string &name = "") {
    if (!parent) {
      ConfigurationGlobalGet().LoggerGet()->LogError("Attempted to create Mask with null parent");
      return 0;
    }
    return new Mask(parent, name);
  }

  bool Mask::MouseMaskingTest(float x, float y) {
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

