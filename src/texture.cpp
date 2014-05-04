
#include "frames/texture.h"

#include "frames/environment.h"

namespace Frames {

  int Texture::RawBPPGet(Format format) {
    if (format == FORMAT_RGBA_8) {
      return 4;
    } else if (format == FORMAT_RGB_8) {
      return 3;
    } else if (format == FORMAT_L_8 || format == FORMAT_A_8) {
      return 1;
    } else {
      // TODO global error
      return 4;
    }
  }

  /*static*/ Ptr<Texture> Texture::CreateRawManaged(Environment *env, int width, int height, Format format) {
    int stride = width * RawBPPGet(format);
    return CreateRawUnmanaged(env, width, height, format, new unsigned char[stride * height], stride, true);
  }

  /*static*/ Ptr<Texture> Texture::CreateRawUnmanaged(Environment *env, int width, int height, Format format, unsigned char *data, int stride, bool takeOwnership /*= false*/) {
    Ptr<Texture> rv(new Texture);
    rv->m_type = RAW;
    rv->m_format = format;
    rv->m_width = width;
    rv->m_height = height;
    rv->m_env = env;
    rv->m_raw_stride = stride;
    rv->m_raw_data = data;
    rv->m_raw_owned = takeOwnership;
    return rv;
  }

  Texture::Texture() : 
    m_type(NIL),
    m_format(FORMAT_RGBA_8),
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
