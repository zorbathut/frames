
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

  /*static*/ const char *Text::GetStaticType() {
    return "Text";
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

  void Text::SetColor(float r, float g, float b, float a /*= 1.0f*/) {
    m_text_r = r;
    m_text_g = g;
    m_text_b = b;
    m_text_a = a;
    // no need to update layout, this hasn't changed the layout at all
  }

  void Text::GetColor(float *r, float *g, float *b, float *a /*= 0*/) {
    *r = m_text_r;
    *g = m_text_g;
    *b = m_text_b;
    if (a) *a = m_text_a;
  }

  Text::Text(Layout *parent) :
      Frame(parent),
      m_size(16),
      m_wordwrap(false),
      m_text_r(1), m_text_g(1), m_text_b(1), m_text_a(1)
  {
    m_font = GetEnvironment()->GetConfiguration().fontDefaultId;

    SetWidthDefault(0);
    SetHeightDefault(20);

    // default font
    UpdateLayout();

    // TODO: We really only need to update fullheight, and that only if wordwrap is enabled. Worry about this later.
    // There's a bit of a potential performance issue here - imagine we have a wordwrapped text field of the wrong (too small) size, with the width set and the height unset.
    // We clear the width.
    // This function triggers.
    // The function will change the default width and height. Now the size has changed again! So we trigger this function again!
    // Now, luckily, all the stuff it does is highly cached - each lookup is essentially two bimap lookups and a little muckery with smart pointers. So this isn't a catastrophe. But still it's not really ideal.
    // Maybe we should cache the TextInfoPtr?
    EventSizeAttach(Delegate<void ()>(this, &Text::UpdateLayout));
  };

  Text::~Text() { };
  
  void Text::UpdateLayout() {
    if (m_font.empty() && m_text.empty()) {
    } else if (m_font.empty()) {
      // PROBLEM
      GetEnvironment()->LogError("Error - attempting to render text without a valid font");
    } else {
      TextInfoPtr tinfo = GetEnvironment()->GetTextManager()->GetTextInfo(m_font, m_size, m_text);
      SetWidthDefault(tinfo->GetFullWidth());

      m_layout = tinfo->GetLayout(GetWidth(), m_wordwrap);
      SetHeightDefault(m_layout->GetFullHeight());
    }
  }

  void Text::RenderElement(Renderer *renderer) const {
    Frame::RenderElement(renderer);

    // we'll fix this up further later
    if (m_layout) {
      // do stuf
      m_layout->Render(renderer, m_text_r, m_text_g, m_text_b, m_text_a, GetBounds());
    }
  }
}

