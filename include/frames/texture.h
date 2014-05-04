// This is what's returned from the configuration to the internal systems

#ifndef TEXTURE_CONFIG
#define TEXTURE_CONFIG

#include "frames/ptr.h"

namespace Frames {
  class Environment;

  class Texture;
  typedef Ptr<Texture> TexturePtr;

  /// Represents the raw data forming a graphics texture.
  /** Used to give data to Frames so Frames can create a texture out of it. This class is intended to provide data in multiple forms, from raw binary to highly compressed hardware textures.
  
  This is currently a temporary class and can be used only to provide initialization data for a texture. */
  class Texture : public Refcountable<Texture> {
  public:
    // ---- Types

    /// Pixel format. Not all formats make sense with all Types.
    enum Format {
      FORMAT_RGBA_8, //< Color data with alpha channel. 8 bits per channel, 32 bits per pixel. Laid out as RGBA.
      FORMAT_RGB_8, //< Color data, no alpha channel. 8 bits per channel, 24 bits per pixel. Laid out as RGB. Packed. Will be converted to RGBA for actual textures. Probably slower than FORMAT_RGBA.
      FORMAT_L_8, //< Luminance only. 8 bits per channel, 8 bits per pixel.
      FORMAT_A_8, //< Alpha only. 8 bits per channel, 8 bits per pixel.
    };

    // ---- Creation

    /// Creates a Texture with a given width, height, and format, with the Texture entirely responsible for allocation and deallocation.
    /** Texture will be created containing all 0 values. Use RawDataGet() to modify the data before using this Texture. */
    static TexturePtr CreateRawManaged(Environment *env, int width, int height, Format format);
    /// Creates a Texture with a given width, height, and format, referencing an existing block of data.
    /** If takeOwnership is true, the Texture will automatically deallocate data using the Environment's allocator. */
    static TexturePtr CreateRawUnmanaged(Environment *env, int width, int height, Format format, unsigned char *data, int stride, bool takeOwnership = false);

    /// Creates a NIL Texture.
    /** This is not valid to pass to any function expecting a Texture. */
    Texture();
    ~Texture();

    // ---- Generic data

    /// Texture type.
    enum Type {
      NIL, //< Invalid texture used as a default value.
      RAW, //< Raw byte array containing decompressed image data.
    };
    /// Returns the Type.
    Type TypeGet() const { return m_type; }

    /// Returns the \ref Format "pixel format".
    Format FormatGet() const { return m_format; }
    /// Returns the width in pixels.
    int WidthGet() const { return m_width; }
    /// Returns the height in pixels.
    int HeightGet() const { return m_height; }
    
    // ---- Raw accessors

    /// Returns the raw data.
    /** Not valid if this Texture type is not RAW. */
    unsigned char *RawDataGet() { return m_raw_data; }
    /// Returns the raw data.
    /** Not valid if this Texture type is not RAW. */
    const unsigned char *RawDataGet() const { return m_raw_data; }
    /// Returns the raw data's stride.
    /** Stride is the memory offset, in bytes, between rows of the texture. On densely-packed textures this will be WidthGet() * RawBPPGet(FormatGet()). It may be larger on textures with row padding. */
    int RawStrideGet() const { return m_raw_stride; }

    // ---- Helper functions

    /// Returns the number of bytes per pixel for a given Format, assuming Raw texture type.
    static int RawBPPGet(Format format);
    
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

