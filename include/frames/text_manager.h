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

#ifndef FRAMES_TEXT_MANAGER
#define FRAMES_TEXT_MANAGER

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/bimap.hpp>

#include "frames/color.h"
#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/rect.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace Frames {
  class Environment;
  class Stream;
  typedef Ptr<Stream> StreamPtr;

  namespace detail {
    class TextureBacking;
    typedef Ptr<TextureBacking> TextureBackingPtr;
    class TextureChunk;
    typedef Ptr<TextureChunk> TextureChunkPtr;

    class FontInfo;
    typedef Ptr<FontInfo> FontInfoPtr;
    class TextInfo;
    typedef Ptr<TextInfo> TextInfoPtr;
    class CharacterInfo;
    typedef Ptr<CharacterInfo> CharacterInfoPtr;
    class TextLayout;
    typedef Ptr<TextLayout> TextLayoutPtr;

    class Renderer;

    // Note: This is all probably a bit overdesigned and needs some cleanup

    // Includes info on a given font
    // Contains a lookup table for (size, character) and one for (size, text)
    // Owns TextInfo and CharacterInfo, is owned by TextManager
    class FontInfo : public Refcountable<FontInfo> {
      friend class Refcountable<FontInfo>;
    public:
      FontInfo(Environment *env, const StreamPtr &stream);

      TextInfoPtr GetTextInfo(float size, const std::string &text);
      CharacterInfoPtr GetCharacterInfo(float size, int character); // character is, as usual, a UCS-4 codepoint

      FT_Face GetFace(float size);

      const TextureBackingPtr &TextureGet() const { return m_texture; }

      void ShutdownText(TextInfo *tinfo);
      void ShutdownCharacter(CharacterInfo *cinfo);

      float GetLineHeight(float size);
      float GetLineHeightFirst(float size);

      float GetKerning(float size, int char1, int char2);

      Environment *EnvironmentGet() { return m_env; }
    private:
      ~FontInfo();

      boost::bimap<std::pair<float, std::string>, TextInfo *> m_text;
      boost::bimap<std::pair<float, int>, CharacterInfo *> m_character;

      // TODO: clear this out when no longer needed?
      struct KerningInfo {
        float size;
        int char1;
        int char2;
        bool operator<(const KerningInfo &rhs) const;
      };
      std::map<KerningInfo, float> m_kerning;

      // it's possible this should be either global or tied to a size, but fuck it
      TextureBackingPtr m_texture;

      Environment *m_env;

      std::vector<unsigned char> m_face_data; // stores the font file in memory to make truetype loading a bit easier

      float m_face_size;
      FT_Face m_face;
    };

    // Includes info on a given (font, size, text) combo
    // Contains a list of active characters for that text as well as the text's fullwidth
    // Is owned by FontInfo
    class TextInfo : public Refcountable<TextInfo> {
      friend class Refcountable<TextInfo>;
    public:
      TextInfo(FontInfoPtr parent, float size, std::string text);

      FontInfo *ParentGet() const { return m_parent.Get(); }
      TextLayoutPtr GetLayout(float width, bool wordwrap);

      float GetFullWidth() const { return m_fullWidth; }
      const TextureBackingPtr &TextureGet() const { return m_parent->TextureGet(); }

      int GetCharacterCount() const { return (int)m_characters.size(); }  // we will never have more than 2 billion characters
      const CharacterInfoPtr &GetCharacter(int index) const { return m_characters[index]; }
      float GetKerning(int index) const { return m_kerning[index]; }
      int GetQuads() const { return m_quads; }

      float SizeGet() const { return m_size; }

      void ShutdownLayout(TextLayout *layout);
    private:
      ~TextInfo();

      FontInfoPtr m_parent;

      boost::bimap<std::pair<float, bool>, TextLayout *> m_layout;

      std::vector<CharacterInfoPtr> m_characters;
      std::vector<float> m_kerning; // difference between character x and character x-1, m_kerning[0] = 0
      float m_fullWidth;
      int m_quads;

      float m_size;
    };

    // Includes info on a given character in a given (font, size) pair
    // Is owned by FontInfo
    class CharacterInfo : public Refcountable<CharacterInfo> {
      friend class Refcountable<CharacterInfo>;
    public:
      CharacterInfo(FontInfoPtr parent, float size, int character);

      const TextureChunkPtr &TextureGet() const { return m_texture; }

      float GetOffsetX() const { return m_offset_x; }
      float GetOffsetY() const { return m_offset_y; }

      float GetAdvance() const { return m_advance; }

      bool IsNewline() const { return m_is_newline; }
      bool IsWordbreak() const { return m_is_wordbreak; }

    private:
      ~CharacterInfo();

      FontInfoPtr m_parent;

      TextureChunkPtr m_texture;

      float m_offset_x;
      float m_offset_y;

      float m_advance;

      bool m_is_newline;
      bool m_is_wordbreak;
    };

    // Includes info on a given (font, size, text, width, wordwrap) quartet
    // Contains rendering instructions for each character
    // Is owned by TextInfo
    class TextLayout : public Refcountable<TextLayout> {
      friend class Refcountable<TextLayout>;
    public:
      TextLayout(TextInfoPtr parent, float width, bool wordwrap);

      TextInfo *ParentGet() const { return m_parent.Get(); }

      float GetFullHeight() const { return m_fullHeight; }

      void Render(Renderer *renderer, const Color &color, Rect bounds, Vector offset); // bounds.s.x and bounds.s.y interpreted as starting coordinates, text clamped to stay within bounds. offset is text's skew within those bounds
      // passed by value because we modify them

      Vector GetCoordinateFromCharacter(int character) const;
      int GetCharacterFromCoordinate(const Vector &pt) const;

      int GetLineFromCharacter(int character) const;
      int GetEOLFromLine(int line) const;

    private:
      ~TextLayout();

      TextInfoPtr m_parent;
      std::vector<Vector> m_coordinates; // contains coordinates for the points in TextInfoPtr
      std::vector<int> m_lines; // first character of each line

      float m_fullHeight;
      float m_width;
    };

    class TextManager : Noncopyable {
    public:
      TextManager(Environment *env);
      ~TextManager();

      TextInfoPtr GetTextInfo(const std::string &font, float size, const std::string &text);

      const FT_Library &GetFreetype() const { return m_ft; }
    private:
      // Allows for accessor function calls
      friend class FontInfo;

      Environment *m_env;
      FT_Library m_ft;

      boost::bimap<std::string, FontInfo *> m_fonts;

      void Internal_Shutdown_Font(FontInfo *font);
    };
  }
}

#endif
