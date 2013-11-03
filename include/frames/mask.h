// The Mask class

#ifndef FRAME_MASK
#define FRAME_MASK

#include "frames/frame.h"

namespace Frames {
  class Mask : public Frame {
    friend class Environment;

  public:
    static Mask *CreateBare(Layout *parent);
    static Mask *CreateTagged_imp(const char *filename, int line, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }

  protected:
    virtual bool TestMouseMasking(float x, float y);

    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, GetStaticType()); Frame::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Mask(Layout *parent);
    virtual ~Mask();

    virtual void RenderElement(Renderer *renderer) const;
    virtual void RenderElementPost(Renderer *renderer) const;
  };
}

#endif
