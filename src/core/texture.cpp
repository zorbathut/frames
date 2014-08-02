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

#include "frames/texture.h"

#include "frames/environment.h"

namespace Frames {

  int Texture::RawBPPGet(Format format) {
    if (format == FORMAT_RGBA_8) {
      return 4;
    } else if (format == FORMAT_RGB_8) {
      return 3;
    } else if (format == FORMAT_R_8) {
      return 1;
    } else {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve texture BPP from invalid Format");
      return 4; // . . . sure, whatever
    }
  }

  /*static*/ TexturePtr Texture::CreateRawManaged(Environment *env, int width, int height, Format format) {
    int stride = width * RawBPPGet(format);
    return CreateRawUnmanaged(env, width, height, format, new unsigned char[stride * height], stride, true);
  }

  /*static*/ TexturePtr Texture::CreateRawUnmanaged(Environment *env, int width, int height, Format format, unsigned char *data, int stride, bool takeOwnership /*= false*/) {
    TexturePtr rv(new Texture);
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

  unsigned char *Texture::RawDataGet() {
    if (m_type != RAW) {
      m_env->LogError("Attempted to retrieve raw data from non-RAW Texture");
    }
    return m_raw_data;
  }
    
  const unsigned char *Texture::RawDataGet() const {
    if (m_type != RAW) {
      m_env->LogError("Attempted to retrieve raw data from non-RAW Texture");
    }
    return m_raw_data;
  }
    
  int Texture::RawStrideGet() const {
    if (m_type != RAW) {
      m_env->LogError("Attempted to retrieve raw stride from non-RAW Texture");
    }
    return m_raw_stride;
  }

  const Texture::ContextualPtr &Texture::ContextualGet() const {
    if (m_type != CONTEXTUAL) {
      m_env->LogError("Attempted to retrieve contextual data from non-CONTEXTUAL Texture");
    }
    return m_contextual;
  }
}
