// The Raw class

#ifndef FRAMES_RAW
#define FRAMES_RAW

#include "frames/frame.h"

namespace Frames {
  FRAMES_FRAMEEVENT_DECLARE(Render, ());

  class Raw : public Frame {
    friend class Environment;

  public:
    static Raw *CreateBare(Layout *parent);
    static Raw *CreateTagged_imp(const char *filename, int line, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }
    
  protected:
    virtual void l_Register(lua_State *L) const { l_RegisterWorker(L, GetStaticType()); Frame::l_Register(L); }

    static void l_RegisterFunctions(lua_State *L);

  private:
    Raw(Layout *parent);
    virtual ~Raw();

    virtual void RenderElement(Renderer *renderer) const;
  };
}

#endif
