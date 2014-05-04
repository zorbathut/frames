// This is what's returned from the configuration to the internal systems

#ifndef TEXTURE_CONFIG
#define TEXTURE_CONFIG

#include "frames/ptr.h"

namespace Frames {
  class Environment;

  class Texture : public Refcountable<Texture> {
  public:
    // ---- Types

    enum Format {
      FORMAT_RGBA, // 8bpc, 32bpp, laid out as RGBA
      FORMAT_RGB, // 8bpc, 24bpp, laid out as RGB. Packed. Will be converted to RGBA for actual textures. Probably slower than RAW_RGBA if conversion happens often.
      FORMAT_L, // 8bpc, 8bpp, laid out as L.
      FORMAT_A, // 8bpc, 8bpp, laid out as A.
    };

    // ---- Creation

    static Ptr<Texture> CreateRawManaged(Environment *env, int width, int height, Format format);
    static Ptr<Texture> CreateRawUnmanaged(Environment *env, int width, int height, Format format, unsigned char *data, int stride);

    Texture();
    ~Texture();

    // ---- Generic data

    enum Type { NIL, RAW };
    Type TypeGet() const { return m_type; }

    Format FormatGet() const { return m_format; }
    int WidthGet() const { return m_width; }
    int HeightGet() const { return m_height; }
    
    // ---- Raw accessors

    unsigned char *RawDataGet() { return m_raw_data; }
    const unsigned char *RawDataGet() const { return m_raw_data; }
    int RawStrideGet() const { return m_raw_stride; }

    // ---- Helper functions

    static int GetBPP(Format format);

    
  private:
    Type m_type;

    Format m_format;
    int m_width;
    int m_height;

    unsigned char *m_raw_data;
    int m_raw_stride;
    bool m_raw_owned;
    

    Environment *m_env;
  };
}

#endif

