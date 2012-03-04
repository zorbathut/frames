
#include "frames/text.h"

#include "frames/environment.h"
#include "frames/rect.h"
#include "frames/renderer.h"
#include "frames/texture_manager.h"

#include <GL/GLew.h>

namespace Frames {
  Text *Text::CreateBare(Layout *parent) {
    return new Text(parent);
  }
  Text *Text::CreateTagged_imp(const char *filename, int line, Layout *parent) {
    Text *rv = new Text(parent);
    rv->SetNameStatic(filename);
    rv->SetNameId(line);
    return rv;
  }

  void Text::SetText(const std::string &text) {
    if (m_text != text) {
      m_text = text;
      
      UpdateLayout();
    }
  }

  void Text::SetFont(const std::string &id) {
    if (m_font != id) {
      m_font = id;

      UpdateLayout();
    }
  }

  void Text::SetSize(float size) {
    if (m_size != size) {
      m_size = size;

      UpdateLayout();
    }
  }

  void Text::SetWordwrap(bool wordwrap) {
    if (m_wordwrap != wordwrap) {
      m_wordwrap = wordwrap;

      UpdateLayout();
    }
  }

  Text::Text(Layout *parent) :
      Frame(parent),
      m_size(16),
      m_font("ParmaPetit-Normal.ttf"),  // hurf durf replace this
      m_wordwrap(false)
  {
    // default font
    UpdateLayout();
  };

  Text::~Text() { };
  
  void Text::UpdateLayout() {
    TextInfoPtr tinfo = GetEnvironment()->GetTextManager()->GetTextInfo(m_font, m_size, m_text);
    SetWidthDefault(tinfo->GetFullWidth());

    m_layout = tinfo->GetLayout(GetWidth(), m_wordwrap);
    SetHeightDefault(m_layout->GetFullHeight());
  }

  void Text::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // we'll fix this up further later
    if (m_layout) {
      // do stuf
      m_layout->Render(renderer, 1, 1, 1, 1, GetBounds());
    }
  }
}

