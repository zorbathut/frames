// The Raw class

#ifndef FRAME_RAW
#define FRAME_RAW

#include "frame/frame.h"

namespace Frame {
  FRAME_FRAMEEVENT_DECLARE(Render, ());

  class Raw : public Frame {
    friend class Environment;

  public:
    static Raw *CreateBare(Layout *parent);
    static Raw *CreateTagged_imp(const char *filename, int line, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }
    
  protected:
    virtual void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, GetStaticType()); Frame::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Raw(Layout *parent);
    virtual ~Raw();

    virtual void RenderElement(Renderer *renderer) const;
  };
}

#endif
