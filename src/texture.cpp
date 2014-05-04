
#include "frames/texture.h"

#include "frames/environment.h"

namespace Frames {

  int Texture::GetBPP(Type type) {
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

  /*static*/ Ptr<Texture> Texture::CreateManagedRaw(Environment *env, int width, int height, Type mode) {
    Ptr<Texture> rv(new Texture);
    rv->m_mode = RAW;
    rv->m_width = width;
    rv->m_height = height;
    rv->m_env = env;
    rv->m_raw_type = mode;
    rv->m_raw_stride = width * GetBPP(mode);
    rv->m_raw_data = new unsigned char[rv->m_raw_stride * rv->m_height];
    rv->m_raw_owned = true;
    return rv;
  }

  /*static*/ Ptr<Texture> Texture::CreateUnmanagedRaw(Environment *env, int width, int height, Type mode, unsigned char *data, int stride) {
    Ptr<Texture> rv(new Texture);
    rv->m_mode = RAW;
    rv->m_width = width;
    rv->m_height = height;
    rv->m_env = env;
    rv->m_raw_type = mode;
    rv->m_raw_stride = stride;
    rv->m_raw_data = data;
    rv->m_raw_owned = false;
    return rv;
  }

  Texture::Texture() : 
    m_mode(NIL),
    m_width(0),
    m_height(0),
    m_raw_data(0),
    m_raw_owned(false),
    m_raw_stride(0),
    m_raw_type(MODE_RGBA),
    m_env(0)
  {
  }

  Texture::~Texture() {
    if (m_raw_owned) {
      delete [] m_raw_data;
    }
  }
}
