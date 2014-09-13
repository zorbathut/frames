/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef FRAMES_TEXT
#define FRAMES_TEXT

#include "frames/detail.h"
#include "frames/frame.h"

namespace Frames {
  namespace detail {
    class TextLayout;
    typedef Ptr<TextLayout> TextLayoutPtr;
  }

  /// Used to render and edit text.
  /** Text can act as a read-only text display, a selectable-but-not-editable text box, or a full editable textbox. It supports optional wordwrap. It can resize itself to fit whatever is currently contained in it.
  
  Note that Text is not intended for huge amounts of text in one frame. If your text can fit on one or two screens, you're fine, but putting an entire novel into it may cause performance issues.*/ // TODO feature request
  class Text : public Frame {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    /// Creates a new Text.
    static Text *Create(Layout *parent, const std::string &name);

    /// Sets the text currently displayed. 
    /** The cursor will be left at the same character location. */
    void TextSet(const std::string &text);
    /// Gets the text currently displayed.
    const std::string &TextGet() const { return m_text; }

    /// Sets the font used to render text, given by ID.
    /** See \ref basicsresources "Resources" for details on Frames's resource system. */
    void FontSet(const std::string &id);
    /// Gets the ID of the font used to render text.
    const std::string &FontGet() const { return m_font; }

    /// Sets the size of the font.
    /** This is currently in arbitrary undefined units - this will change at some point in the future. */
    void FontSizeSet(float size);
    /// Gets the size of the font.
    float FontSizeGet() const { return m_format.size; }

    // Experimental, disabled for documentation
    /// @cond EXPERIMENTAL
    /// Sets whether the font's fill should be rendered.
    void EXPERIMENTAL_FillSet(bool fill);
    /// Gets whether the font's fill should be rendered.
    bool EXPERIMENTAL_FillGet() const { return m_format.fill; }

    /// Sets whether the font's stroke should be rendered.
    /** This is currently an experimental feature. It is not yet clear how this should affect the text element's bounds - as of this writing, it will add StrokeSize units on all sides of the text for proper padding. */
    void EXPERIMENTAL_StrokeSet(bool stroke);
    /// Gets whether the font's stroke should be rendered.
    bool EXPERIMENTAL_StrokeGet() const { return m_format.stroke; }

    /// Sets the size of the stroke radius.
    /** Same units as FontSize. */
    void EXPERIMENTAL_StrokeSizeSet(float radius);
    /// Gets the size of the stroke radius.
    float EXPERIMENTAL_StrokeSizeGet() const { return m_format.strokeRadius; }

    /// Sets the cap type of the stroke.
    /** This controls how the stroke deals with the ends of lines. */
    void EXPERIMENTAL_StrokeCapSet(FontStrokeCap cap);
    /// Gets the cap type of the stroke.
    FontStrokeCap EXPERIMENTAL_StrokeCapGet() const { return m_format.strokeCap; }

    /// Sets the join type of the stroke.
    /** This controls how the stroke deals with corners. */
    void EXPERIMENTAL_StrokeJoinSet(FontStrokeJoin join);
    /// Gets the join type of the stroke.
    FontStrokeJoin EXPERIMENTAL_StrokeJoinGet() const { return m_format.strokeJoin; }

    /// Sets the miter limit of the stroke.
    /** This controls how the stroke deals with sharp corners.
    Larger numbers will result in longer spikes at extremely sharp corners, short numbers will truncate those spikes more closely to the letter.
    Interpreted as a multiple of StrokeSize.  */
    void EXPERIMENTAL_StrokeMiterlimitSet(float limit);
    /// Gets the join type of the stroke.
    float EXPERIMENTAL_StrokeMiterlimitGet() const { return m_format.strokeMiterLimit; }
    /// @endcond

    /// Sets the wordwrap flag.
    /** If wordwrap is enabled, text will wrap at the width of the text frame. If wordwrap is disabled, the text frame will be able to scroll horizontally. */
    void WordwrapSet(bool wordwrap);
    /// Gets the wordwrap flag.
    bool WordwrapGet() const { return m_wordwrap; }

    /// Sets the color of the text.
    void ColorTextSet(const Color &color);
    /// Gets the color of the text.
    const Color &ColorTextGet() const { return m_color_text; }
    
    /// List of interactivity settings allowed.
    enum InteractivityMode {
      INTERACTIVE_NONE, ///< No interactivity at all.
      INTERACTIVE_SELECT, ///< Allows text selection with the mouse, plus copy to clipboard.
      INTERACTIVE_CURSOR, ///< Allows a cursor, text selection with mouse and keyboard, and copy to clipboard.
      INTERACTIVE_EDIT, ///< Allows full text editing with mouse and keyboard.
    };
    /// Sets the interactivity mode.
    void InteractiveSet(InteractivityMode interactive);
    /// Gets the interactivity mode.
    InteractivityMode InteractiveGet() const { return m_interactive; }

    /// Sets the cursor position.
    /** Cursor position is done in terms of the byte that the cursor is placed directly before.
    0 places the cursor at the beginning of the textfield; TextGet().size() places the cursor at the end of the textfield.
    Placing the character within a multibyte code point is undefined behavior.
    
    Also scrolls the textbox as appropriate to keep the cursor in view. */
    void CursorSet(int position);
    /// Gets the cursor position.
    int CursorGet() const { return m_cursor; }

    /// Unselects all text.
    void SelectionClear();
    /// Selects the given text range.
    /** See CursorSet for cursor semantics.
    
    If the cursor was not already at the start or end of the selection, places the cursor at the end. */
    void SelectionSet(int start, int end);
    /// Gets whether text is selected or not.
    bool SelectionActiveGet() const;
    /// Gets the beginning of the active selection.
    /** May not be called if no selection is active. */
    int SelectionBeginGet() const;
    /// Gets the end of the active selection.
    /** May not be called if no selection is active. */
    int SelectionEndGet() const;

    /// Sets the scroll position of the text field.
    /** The Text field will be translated by the inverse of this, such that the text at this coordinate will now be at the top-left corner. */
    void ScrollSet(const Vector &scroll);
    /// Gets the scroll position of the text field.
    const Vector &ScrollGet() const { return m_scroll; }

    /// Sets the background color of the text-selection box.
    void ColorSelectionSet(const Color &color);
    /// Gets the background color of the text-selection box.
    const Color &ColorSelectionGet() const { return m_color_selection; }

    /// Sets the color of selected text.
    void ColorTextSelectedSet(const Color &color);
    /// Gets the color of selected text.
    const Color &ColorTextSelectedGet() const { return m_color_selected; }

  protected:
    /// Creates a new Text with the given parameters. "parent" must be non-null.
    Text(Layout *parent, const std::string &name);
    virtual ~Text() FRAMES_OVERRIDE;

    /// Renders the Text.
    virtual void RenderElement(detail::Renderer *renderer) const FRAMES_OVERRIDE;

  private:
  
    void SizeChanged(Handle *handle);
    void UpdateLayout();
    void ScrollToCursor();

    std::string m_font;
    std::string m_text;
    detail::TextFormat m_format;
    bool m_wordwrap;
    detail::TextLayoutPtr m_layout;

    Color m_color_text;
    Color m_color_selection;
    Color m_color_selected;

    InteractivityMode m_interactive;

    Vector m_scroll;

    int m_select;
    int m_cursor;

    // Event handlers for mouse events
    void EventInternal_LeftDown(Handle *e);
    void EventInternal_LeftUp(Handle *e);
    void EventInternal_Move(Handle *e, const Vector &pt);

    // Event handlers for key events
    void EventInternal_KeyText(Handle *e, const std::string &type);
    void EventInternal_KeyDownOrRepeat(Handle *e, Input::Key key);
  };
}

#endif
