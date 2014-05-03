// The Mask class

#ifndef FRAMES_MASK
#define FRAMES_MASK

#include "frames/frame.h"

namespace Frames {
  class Mask : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    static Mask *Create(const std::string &name, Layout *parent);

    static const char *TypeStaticGet();
    virtual const char *TypeGet() const { return TypeStaticGet(); }

  protected:
    virtual bool MouseMaskingTest(float x, float y);

    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, TypeStaticGet()); Frame::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Mask(const std::string &name, Layout *parent);
    virtual ~Mask();

    virtual void RenderElementPreChild(detail::Renderer *renderer) const;
    virtual void RenderElementPostChild(detail::Renderer *renderer) const;
  };
}

#endif
