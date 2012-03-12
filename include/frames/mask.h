// The Mask class

#ifndef FRAMES_MASK
#define FRAMES_MASK

#include "frames/frame.h"

namespace Frames {
  class Mask : public Frame {
  public:
    static Mask *CreateBare(Layout *parent);
    static Mask *CreateTagged_imp(const char *filename, int line, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }

  protected:
    virtual void l_Register(lua_State *L) const { l_RegisterWorker(L, GetStaticType()); Frame::l_Register(L); }

  private:
    Mask(Layout *parent);
    virtual ~Mask();

    virtual void RenderElement(Renderer *renderer) const;
    virtual void RenderElementPost(Renderer *renderer) const;
  };
}

#endif
