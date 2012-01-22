// The Layout class

#ifndef FRAMES_LAYOUT
#define FRAMES_LAYOUT

#include "frames/noncopyable.h"

namespace Frames {
  class Layout : Noncopyable {

  private:
    friend class Environment;

    Layout();  // We'll need more here
    ~Layout();

    void Render();
    virtual void RenderElement() { };
  };

  // implementation detail for LayoutPtr and the like, do not call directly!
  void intrusive_ptr_add_ref(Frames::Layout *layout);
  void intrusive_ptr_release(Frames::Layout *layout);
}

#endif
