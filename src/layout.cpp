
#include "frames/layout.h"

namespace Frames {
  void Layout::Render() {
    RenderElement();
    // render children here
  }

  void intrusive_ptr_add_ref(Frames::Layout *layout) { }
  void intrusive_ptr_release(Frames::Layout *layout) { }
}

