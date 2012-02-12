// The Text class

#ifndef FRAMES_TEXT
#define FRAMES_TEXT

#include "frames/frame.h"

#include "frames/texture_manager.h"

namespace Frames {
  class Text : public Frame {
  public:
    static Text *CreateBare(Layout *parent);
    static Text *CreateTagged_imp(const char *filename, int line, Layout *parent);

    void SetText(const std::string &text);

  private:
    Text(Layout *parent);
    virtual ~Text();
  
    void UpdateDefaultSize();

    virtual void RenderElement(Renderer *renderer) const;

    std::string m_text;
    TextureChunkPtr m_texture;
  };
}

#endif
