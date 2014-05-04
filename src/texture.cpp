
#include "frames/texture.h"

#include "frames/environment.h"

namespace Frames {

  int Texture::GetBPP(Format format) {
    if (format == FORMAT_RGBA) {
      return 4;
    } else if (format == FORMAT_RGB) {
      return 3;
    } else if (format == FORMAT_L || format == FORMAT_A) {
      return 1;
    } else {
      // TODO global error
      return 4;
    }
  }

  /*static*/ Ptr<Texture> Texture::CreateRawManaged(Environment *env, int width, int height, Format format) {
    Ptr<Texture> rv(new Texture);
    rv->m_type = RAW;
    rv->m_format = format;
    rv->m_width = width;
    rv->m_height = height;
    rv->m_env = env;
    rv->m_raw_stride = width * GetBPP(format);
    rv->m_raw_data = new unsigned char[rv->m_raw_stride * rv->m_height];
    rv->m_raw_owned = true;
    return rv;
  }

  /*static*/ Ptr<Texture> Texture::CreateRawUnmanaged(Environment *env, int width, int height, Format format, unsigned char *data, int stride) {
    Ptr<Texture> rv(new Texture);
    rv->m_type = RAW;
    rv->m_format = format;
    rv->m_width = width;
    rv->m_height = height;
    rv->m_env = env;
    rv->m_raw_stride = stride;
    rv->m_raw_data = data;
    rv->m_raw_owned = false;
    return rv;
  }

  Texture::Texture() : 
    m_type(NIL),
    m_format(FORMAT_RGBA),
    m_width(0),
    m_height(0),
    m_raw_data(0),
    m_raw_owned(false),
    m_raw_stride(0),
    m_env(0)
  {
  }

  Texture::~Texture() {
    if (m_raw_owned) {
      delete [] m_raw_data;
    }
  }
}
