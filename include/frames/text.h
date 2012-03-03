// The Text class

#ifndef FRAMES_TEXT
#define FRAMES_TEXT

#include "frames/frame.h"

#include "frames/text_manager.h"

namespace Frames {
  class Text : public Frame {
  public:
    static Text *CreateBare(Layout *parent);
    static Text *CreateTagged_imp(const char *filename, int line, Layout *parent);

    void SetText(const std::string &text);
    const std::string &GetText() const { return m_text; }

    void SetFont(const std::string &id);
    const std::string &GetFont() const { return m_font; }

    void SetSize(float size);
    float GetSize() const { return m_size; }

    void SetWordwrap(bool wordwrap);
    bool GetWordwrap() const { return m_wordwrap; }

  private:
    Text(Layout *parent);
    virtual ~Text();
  
    void UpdateLayout();

    virtual void RenderElement(Renderer *renderer) const;

    std::string m_font;
    std::string m_text;
    float m_size;
    bool m_wordwrap;
    TextLayoutPtr m_layout;
  };
}

#endif
