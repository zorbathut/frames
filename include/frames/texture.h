// This is what's returned from the configuration to the internal systems

#ifndef TEXTURE_CONFIG
#define TEXTURE_CONFIG

namespace Frames {
  class Environment;

  class Texture {
  public:
    enum Type {
      MODE_RGBA, // 8bpc, 32bpp, laid out as RGBA
      MODE_RGB, // 8bpc, 24bpp, laid out as RGB. Packed. Will be converted to RGBA for actual textures. Probably slower than RAW_RGBA if conversion happens often.
      MODE_L, // 8bpc, 8bpp, laid out as L.
      MODE_A, // 8bpc, 8bpp, laid out as A.
    };
    static int GetBPP(Type type);

    enum Mode { NIL, RAW };

    static Texture CreateManagedRaw(Environment *env, int width, int height, Type mode);
    static Texture CreateUnmanagedRaw(Environment *env, int width, int height, Type mode, unsigned char *data, int stride);

    Texture();
    Texture(const Texture &rhs);
    ~Texture();

    void operator=(const Texture &rhs);

    Mode GetMode() const { return m_mode; }

    int WidthGet() const { return m_width; }
    int HeightGet() const { return m_height; }

    unsigned char *Raw_GetData() { return m_raw_data; }
    const unsigned char *Raw_GetData() const { return m_raw_data; }
    int Raw_GetStride() const { return m_raw_stride; }
    Type Raw_TypeGet() const { return m_raw_type; }

  private:
    Mode m_mode;

    int m_width;
    int m_height;

    unsigned char *m_raw_data;
    int *m_raw_refcount;  // null for not owned, otherwise pointer to shared refcount
    int m_raw_stride;
    Type m_raw_type;

    Environment *m_env;
  };
}

#endif

