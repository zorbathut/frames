
#include "frames/texture_config.h"

#include "frames/environment.h"

namespace Frames {

  int TextureConfig::GetBPP(Type type) {
    if (type == MODE_RGBA) {
      return 4;
    } else if (type == MODE_RGB) {
      return 3;
    } else if (type == MODE_L || type == MODE_A) {
      return 1;
    } else {
      // throw error here?
      return 4;
    }
  }

  /*static*/ TextureConfig TextureConfig::CreateManagedRaw(Environment *env, int width, int height, Type mode) {
    TextureConfig rv;
    rv.m_mode = RAW;
    rv.m_width = width;
    rv.m_height = height;
    rv.m_env = env;
    rv.m_raw_type = mode;
    rv.m_raw_stride = width * GetBPP(mode);
    rv.m_raw_data = new unsigned char[rv.m_raw_stride * rv.m_height];
    rv.m_raw_refcount = new int(1);
    return rv;
  }

  /*static*/ TextureConfig TextureConfig::CreateUnmanagedRaw(Environment *env, int width, int height, Type mode, unsigned char *data, int stride) {
    TextureConfig rv;
    rv.m_mode = RAW;
    rv.m_width = width;
    rv.m_height = height;
    rv.m_env = env;
    rv.m_raw_type = mode;
    rv.m_raw_stride = stride;
    rv.m_raw_data = data;
    rv.m_raw_refcount = 0;
    return rv;
  }

  TextureConfig::TextureConfig() : 
    m_mode(NIL),
    m_width(0),
    m_height(0),
    m_raw_data(0),
    m_raw_refcount(0),
    m_raw_stride(0),
    m_raw_type(MODE_RGBA),
    m_env(0)
  {
  }

  TextureConfig::TextureConfig(const TextureConfig &rhs) :
    m_mode(NIL),
    m_width(0),
    m_height(0),
    m_raw_data(0),
    m_raw_refcount(0),
    m_raw_stride(0),
    m_raw_type(MODE_RGBA),
    m_env(0)
  {
    *this = rhs;  // just easier to do it this way
  }

  TextureConfig::~TextureConfig() {
    if (m_mode == NIL) {
    } else if (m_mode == RAW) {
      if (m_raw_refcount) {
        --*m_raw_refcount;
        if (!*m_raw_refcount) {
          delete m_raw_refcount;
          delete [] m_raw_data;
        }
      }
    } else {
      if (m_env) {
        m_env->LogError("Unknown TextureConfig type");
      }
    }
  }

  void TextureConfig::operator=(const TextureConfig &rhs) {
    // first, deallocate if needed
    if (m_mode == NIL) {
    } else if (m_mode == RAW) {
      if (m_raw_refcount) {
        --*m_raw_refcount;
        if (!*m_raw_refcount) {
          delete m_raw_refcount;
          delete [] m_raw_data;
        }
      }
      m_raw_data = 0;
      m_raw_refcount = 0;
      m_raw_stride = 0;
      m_raw_type = MODE_RGBA;
    } else {
      if (m_env) {
        m_env->LogError("Unknown TextureConfig type");
      }
    }

    m_mode = rhs.m_mode;

    m_width = rhs.m_width;
    m_height = rhs.m_height;

    m_env = rhs.m_env; // not sure I like the idea of changing envs, but

    if (m_mode == NIL) {
    } else if (m_mode == RAW) {
      m_raw_data = rhs.m_raw_data;
      m_raw_refcount = rhs.m_raw_refcount;
      m_raw_stride = rhs.m_raw_stride;
      m_raw_type = rhs.m_raw_type;
      if (m_raw_refcount) {
        ++*m_raw_refcount;
      }
    } else {
      if (m_env) {
        m_env->LogError("Unknown TextureConfig type");
      }
    }
  }
}

