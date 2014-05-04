
#include "frames/text_manager.h"

#include "frames/configuration.h"
#include "frames/detail_format.h"
#include "frames/environment.h"
#include "frames/renderer.h"
#include "frames/stream.h"
#include "frames/texture.h"
#include "frames/texture_manager.h"

namespace Frames {
  namespace detail {
    // =======================================
    // FONTINFO

    FontInfo::FontInfo(Environment *env, const StreamPtr &stream) : m_env(env), m_face_size(0) {
      // Hacky fallback for now: read the entire stream to a vector, then we just point to the vector. Later we'll do actual stream reading.
      if (stream) {
        while (true) {
          const int BUFFERSIZE = 32768;
          unsigned char buffer[BUFFERSIZE];
          int read = (int)stream->Read(buffer, BUFFERSIZE); // cast is safe, buffersize will never be 64-bit
          if (read) {
            m_face_data.insert(m_face_data.end(), buffer, buffer + read);
          }
          if (read != BUFFERSIZE) {
            break;
          }
        }
      }

      if (m_face_data.empty() || FT_New_Memory_Face(m_env->GetTextManager()->GetFreetype(), &m_face_data[0], (int)m_face_data.size(), 0, &m_face)) {
        m_env->LogError("Can't load Freetype face");
      }
    
      m_texture = m_env->GetTextureManager()->BackingCreate(1024, 1024, GL_ALPHA);
    }

    FontInfo::~FontInfo() {
      if (FT_Done_Face(m_face)) {
        m_env->LogError("Can't close Freetype face");
      }

      m_env->GetTextManager()->Internal_Shutdown_Font(this);
    }

    TextInfoPtr FontInfo::GetTextInfo(float size, const std::string &text) {
      if (!m_text.left.count(std::make_pair(size, text))) {
        m_text.insert(boost::bimap<std::pair<float, std::string>, TextInfo *>::value_type(std::make_pair(size, text), new TextInfo(FontInfoPtr(this), size, text)));
      }

      return TextInfoPtr(m_text.left.find(std::make_pair(size, text))->second);
    }

    CharacterInfoPtr FontInfo::GetCharacterInfo(float size, int character) {
      if (!m_character.left.count(std::make_pair(size, character))) {
        m_character.insert(boost::bimap<std::pair<float, int>, CharacterInfo *>::value_type(std::make_pair(size, character), new CharacterInfo(FontInfoPtr(this), size, character)));
      }

      return CharacterInfoPtr(m_character.left.find(std::make_pair(size, character))->second);
    }

    FT_Face FontInfo::GetFace(float size) {
      if (m_face_size != size) {
        FT_Size_RequestRec rec;
        rec.type = FT_SIZE_REQUEST_TYPE_REAL_DIM;
        rec.width = (int)(size * 64 + 0.5f) - 64;
        rec.height = rec.width;
        rec.horiResolution = 0;
        rec.vertResolution = 0;
        FT_Request_Size(m_face, &rec);
        m_face_size = size;

        //m_env->LogDebug(detail::Format("Generated size %f, requested %d/%d, lineheight is %f, calculated from %d/%d", size, rec.width, rec.height, GetLineHeightFirst(size), m_face->size->metrics.ascender, m_face->size->metrics.descender));
      }

      return m_face;
    }

    void FontInfo::ShutdownText(TextInfo *tinfo) {
      m_text.right.erase(tinfo);
    }

    void FontInfo::ShutdownCharacter(CharacterInfo *cinfo) {
      m_character.right.erase(cinfo);
    }

    float FontInfo::GetLineHeight(float size) {
      FT_Face face = GetFace(size);
      return face->size->metrics.height / 64.0f;
    }

    float FontInfo::GetLineHeightFirst(float size) {
      FT_Face face = GetFace(size);
      return (face->size->metrics.ascender - face->size->metrics.descender) / 64.0f;
    }

    float FontInfo::GetKerning(float size, int char1, int char2) {
      KerningInfo kinfo;
      kinfo.size = size;
      kinfo.char1 = char1;
      kinfo.char2 = char2;

      std::map<KerningInfo, float>::iterator found = m_kerning.find(kinfo);
      if (found != m_kerning.end()) {
        return found->second;
      } else {
        FT_Face face = GetFace(size);

        FT_UInt index_prev = FT_Get_Char_Index(face, char1);
        FT_UInt index_current = FT_Get_Char_Index(face, char2);

        FT_Vector delta;
        FT_Get_Kerning(face, index_prev, index_current, FT_KERNING_DEFAULT, &delta);

        float kern = std::floor(delta.x / 64.f + 0.5f);

        m_kerning[kinfo] = kern;
        return kern;
      }
    }

    bool FontInfo::KerningInfo::operator<(const KerningInfo &rhs) const {
      if (char1 != rhs.char1) return char1 < rhs.char1;
      if (char2 != rhs.char2) return char2 < rhs.char2;
      return size < rhs.size;
    }

    // =======================================
    // TEXTINFO

    TextInfo::TextInfo(FontInfoPtr parent, float size, std::string text) : m_parent(parent), m_fullWidth(0), m_quads(0), m_size(size) {
      // TODO: Unicode!
      float linewidth = 0;
      float lastadjust = 0;
      for (int i = 0; i < (int)text.size(); ++i) {
        m_characters.push_back(m_parent->GetCharacterInfo(size, text[i]));

        if (!i) {
          m_kerning.push_back(0.f);
        } else {
          float kerning = m_parent->GetKerning(size, text[i - 1], text[i]);

          m_kerning.push_back(kerning);

          if (linewidth > 0) {
            linewidth += kerning;
          }
        }

        if (text[i] == '\n') {
          m_fullWidth = std::max(m_fullWidth, linewidth + lastadjust);
          linewidth = 0;
          lastadjust = 0;
        } else {
          linewidth += m_characters.back()->GetAdvance();
          if (m_characters.back()->TextureGet()) {
            lastadjust = m_characters.back()->GetAdvance() - m_characters.back()->TextureGet()->WidthGet();
          } else {
            lastadjust = m_characters.back()->GetAdvance();
          }
        }

        m_quads += !!m_characters.back()->TextureGet();
      }
      m_fullWidth = std::max(m_fullWidth, linewidth + lastadjust);
    }

    TextInfo::~TextInfo() {
      m_parent->ShutdownText(this);
    }

    TextLayoutPtr TextInfo::GetLayout(float width, bool wordwrap) {
      if (width > m_fullWidth) width = m_fullWidth; // may as well clamp

      if (!m_layout.left.count(std::make_pair(width, wordwrap))) {
        m_layout.insert(boost::bimap<std::pair<float, bool>, TextLayout *>::value_type(std::make_pair(width, wordwrap), new TextLayout(TextInfoPtr(this), width, wordwrap)));
      }

      return TextLayoutPtr(m_layout.left.find(std::make_pair(width, wordwrap))->second);
    }

    void TextInfo::ShutdownLayout(TextLayout *layout) {
      m_layout.right.erase(layout);
    }

    // =======================================
    // CHARACTERINFO

    CharacterInfo::CharacterInfo(FontInfoPtr parent, float size, int character) : m_parent(parent), m_offset_x(0), m_offset_y(0), m_advance(0), m_is_newline(false), m_is_wordbreak(false) {
      // LET'S DO THIS THING
      m_is_newline = (character == '\n');
      m_is_wordbreak = (isspace(character) != 0);

      FT_Face face = parent->GetFace(size);

      FT_UInt glyph_index = FT_Get_Char_Index(face, character);

      if (FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_HINTING))
        return;

      FT_Glyph glyph;
      
      if (FT_Get_Glyph(face->glyph, &glyph))
        return;

      m_advance = (float)(int)std::floor((float)face->glyph->advance.x / 64.f + 0.5f);  // this should probably be cleaned up - how *do* we want to deal with pixel subsampling?

      if (isspace(character))
        return; // we don't need bitmaps for whitespace characters

      if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 0))
        return;

      FT_BitmapGlyph bmp = (FT_BitmapGlyph)glyph;

      if (bmp && bmp->bitmap.buffer) {
        m_texture = parent->EnvironmentGet()->GetTextureManager()->TextureFromConfig(
              Texture::CreateRawUnmanaged(parent->EnvironmentGet(), bmp->bitmap.width, bmp->bitmap.rows, Texture::FORMAT_A_8, bmp->bitmap.buffer, bmp->bitmap.width),
              parent->TextureGet()
        );

        m_offset_x = (float)bmp->left;
        m_offset_y = -bmp->top + face->size->metrics.ascender / 64.f;
      }

      FT_Done_Glyph(glyph);
    }

    CharacterInfo::~CharacterInfo() {
      m_parent->ShutdownCharacter(this);
    }

    // =======================================
    // TEXTLAYOUT

    TextLayout::TextLayout(TextInfoPtr parent, float width, bool wordwrap) : m_parent(parent), m_width(width) {
      // we're not doing word wrapping quite yet, soooo
    
      // Word wrapping algorithm:
      // Make a list of characters representing the current word, along with the prefix starting the word (if appropriate)
      // Once we're done with a word, try adding it to this line
      // If we fail, chop off the line and add it to the next line
    
      float currentWordStartX = 0;
      int currentWordStartIndex = 0;

      float tx = 0;
      float ty = 0;

      for (int i = 0; i < m_parent->GetCharacterCount(); ++i) {
        CharacterInfo *chr = m_parent->GetCharacter(i).Get();

        if (chr->IsNewline()) {
          // This line is resolved, kill it and move on
          currentWordStartX = 0;
          currentWordStartIndex = i + 1;
          m_lines.push_back(i + 1);

          // important that this happpens here so that the editfield works as intended
          m_coordinates.push_back(Vector(tx, ty));

          tx = 0;
          ty = ty + m_parent->ParentGet()->GetLineHeight(m_parent->SizeGet());
          ty = (float)(int)std::floor(ty + 0.5f); // again we're back to the pixel subsampling nightmare
        
          continue;
        }

        if (currentWordStartIndex != i) {
          // don't kern if we're the first letter in the sentence
          tx += m_parent->GetKerning(i);
        }

        // calculate the distance this character will go
        float charbound = tx;
        if (chr->TextureGet()) {
          charbound += chr->TextureGet()->WidthGet();
        }

        if (wordwrap && charbound > width) {
          // We have a line break, wordwrap as appropriate
          if (currentWordStartX == 0) {
            // This is a bit of a problem - this word is too long.
            if (currentWordStartIndex == i) {
              // In fact it's even worse - this is the first letter.
              // If it's the first letter, we just keep it and move on anyway - it will be too large for the text field, but fuck it, you've given us a half-character-wide textfield, we can't exactly do better.
              // We'll wordwrap on the next character.
              m_coordinates.push_back(Vector(tx, ty));
              tx += chr->GetAdvance();
            } else {
              // Word is too long, but this is the middle of the word.
              // Do a forced line break, take this character, drop it at the beginning of the next word.
              // If the word is too long and this character is too long, then we just plop it down anyway because we don't have a realistic choice.
              tx = 0;
              ty = ty + m_parent->ParentGet()->GetLineHeight(m_parent->SizeGet());
              ty = (float)(int)std::floor(ty + 0.5f);

              m_coordinates.push_back(Vector(tx, ty));
              tx += chr->GetAdvance();

              currentWordStartX = 0;
              currentWordStartIndex = i;
              m_lines.push_back(i);
            }
          } else {
            // This word isn't too long, so we'll transplant the entire word to the next line. We know this will work without linewrapping because it was long enough to fit on this line.
            currentWordStartX = 0;
            tx = 0;
            ty = ty + m_parent->ParentGet()->GetLineHeight(m_parent->SizeGet());
            ty = (float)(int)std::floor(ty + 0.5f);
            m_lines.push_back(currentWordStartIndex);

            for (int j = currentWordStartIndex; j < (int)m_coordinates.size(); ++j) {
              m_coordinates[j] = Vector(tx, ty);
              tx += m_parent->GetCharacter(j)->GetAdvance();
              tx += m_parent->GetKerning(j + 1);  // no kerning on the first character
            }

            // don't need to kern here, we just did it
            m_coordinates.push_back(Vector(tx, ty));
            tx += chr->GetAdvance();
          }
        } else {
          // Push the character, update our X position
          m_coordinates.push_back(Vector(tx, ty));
          tx += chr->GetAdvance();
        }

        if (chr->IsWordbreak()) {
          // This word is resolved
          currentWordStartX = tx;
          currentWordStartIndex = i + 1;
        }
      }

      // Go through and add our character offsets
      for (int i = 0; i < (int)m_coordinates.size(); ++i) {
        m_coordinates[i].x += m_parent->GetCharacter(i)->GetOffsetX();
        m_coordinates[i].y += m_parent->GetCharacter(i)->GetOffsetY();
      }

      m_coordinates.push_back(Vector(tx, ty)); // TODO: remove, generate when generating cursor positions? Or use an entirely separate lookup?

      m_fullHeight = ty + m_parent->ParentGet()->GetLineHeightFirst(m_parent->SizeGet());
    }

    TextLayout::~TextLayout() {
      m_parent->ShutdownLayout(this);
    }

    void TextLayout::Render(Renderer *renderer, const Color &color, Rect bounds, Vector offset) {
      // clamp the bounds to the pixel grid to avoid text blurring
      // probably shouldn't do this if we're using distance field rendering
      bounds.s.x = (float)(int)std::floor(bounds.s.x + 0.5f);
      bounds.s.y = (float)(int)std::floor(bounds.s.y + 0.5f);
      bounds.e.x = (float)(int)std::floor(bounds.e.x + 0.5f);
      bounds.e.y = (float)(int)std::floor(bounds.e.y + 0.5f);
      offset.x = (float)(int)std::floor(offset.x + 0.5f);
      offset.y = (float)(int)std::floor(offset.y + 0.5f);

      // todo: maybe precache this stuff so it becomes a memcpy?
      renderer->TextureSet(m_parent->TextureGet().Get());

      Renderer::Vertex *vertexes = renderer->Request(m_parent->GetQuads() * 4);

      int cquad = 0;
      for (int i = 0; i < (int)m_coordinates.size() - 1; ++i) {
        Renderer::Vertex *vertex = vertexes + cquad * 4;
        const CharacterInfoPtr &character = m_parent->GetCharacter(i);

        if (character->TextureGet()) {
          Vector origin = Vector(bounds.s.x + m_coordinates[i].x - offset.x, bounds.s.y + m_coordinates[i].y - offset.y);
        
          if (Renderer::WriteCroppedTexRect(vertex, Rect(origin, origin + Vector((float)character->TextureGet()->WidthGet(), (float)character->TextureGet()->HeightGet())), character->TextureGet()->BoundsGet(), color, bounds)) {
            cquad++;
          }
        }

        // todo, crop to bounds
      }

      renderer->Return(GL_QUADS, cquad * 4);
    }

    Vector TextLayout::GetCoordinateFromCharacter(int character) const {
      Vector coord = m_coordinates[character];
      if (character + 1 != (int)m_coordinates.size()) {
        CharacterInfo *chr = ParentGet()->GetCharacter(character).Get();
        coord.x -= chr->GetOffsetX();
        coord.y -= chr->GetOffsetY();
      }
      return coord;
    }

    int TextLayout::GetCharacterFromCoordinate(const Vector &pt) const {
      // yeah we're just going to go mad right here
      Vector lpt = detail::Clamp(pt, Vector(0, 0), Vector(m_width, m_fullHeight));
      int line = std::min(int(lpt.y / ParentGet()->ParentGet()->GetLineHeight(ParentGet()->SizeGet())), (int)m_lines.size());

      int s = 0;
      if (line)
        s = m_lines[line - 1];
      int e = GetEOLFromLine(line);
      for (int i = s; i < e; ++i) {
        if (GetCoordinateFromCharacter(i).x >= lpt.x) {
          return i; // this is gonna be a little off, but it's close enough for now
        }
      }
      return e;
    }

    int TextLayout::GetLineFromCharacter(int character) const {
      int line = 0;
      while (line < (int)m_lines.size() && character > m_lines[line]) ++line;
      return line;
    }
    int TextLayout::GetEOLFromLine(int line) const {
      if (line == (int)m_lines.size()) return (int)m_coordinates.size() - 1;
      return m_lines[line] - 1;
    }

    // =======================================
    // TEXTMANAGER

    TextManager::TextManager(Environment *env) : m_env(env) {
      if (FT_Init_FreeType(&m_ft)) {
        m_env->LogError("Cannot initialize freetype");
      }
    }

    TextManager::~TextManager() {
      if (!m_fonts.empty()) {
        // something is wrong
        m_env->LogError("Text manager not empty on shutdown");

        // I guess we leak for now, TODO clean this up more - I think we need more definitive ownership semantics
      }

      if (FT_Done_FreeType(m_ft)) {
        m_env->LogError("Error shutting down freetype");
      }
    }

    TextInfoPtr TextManager::GetTextInfo(const std::string &font, float size, const std::string &text) {
      if (!m_fonts.left.count(font)) {
        StreamPtr stream = m_env->ConfigurationGet().StreamFromIdGet()->Create(m_env, font);
        if (!stream) {
          m_env->LogError(Frames::detail::Format("Unable to find font \"%s\"", font));
          m_fonts.insert(boost::bimap<std::string, FontInfo *>::value_type(font, new FontInfo(m_env, StreamPtr())));
        } else {
          m_fonts.insert(boost::bimap<std::string, FontInfo *>::value_type(font, new FontInfo(m_env, stream)));
        }
      }

      return m_fonts.left.find(font)->second->GetTextInfo(size, text);
    }
  
    void TextManager::Internal_Shutdown_Font(FontInfo *font) {
      m_fonts.right.erase(font);
    }
  }
}

