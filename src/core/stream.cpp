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

#include "frames/stream.h"

#include <algorithm>
#include <cstring>

namespace Frames {
  StreamFilePtr StreamFile::Create(const std::string &fname) {
    std::FILE *fil = fopen(fname.c_str(), "rb");
    if (!fil) return StreamFilePtr();
    return StreamFilePtr(new StreamFile(fil));
  }

  int64_t StreamFile::Read(unsigned char *target, int64_t bytes) { return fread(target, 1, (size_t)bytes, m_file); } // TODO deal with 64-bit file sizes someday
  bool StreamFile::Seek(int64_t offset) { return !fseek(m_file, (long)offset, SEEK_SET); }
  bool StreamFile::Seekable() const { return true; }

  StreamFile::StreamFile(std::FILE *file) : m_file(file) { }
  StreamFile::~StreamFile() { fclose(m_file); }
  
  StreamBufferPtr StreamBuffer::Create(const std::vector<unsigned char> &data) {
    return StreamBufferPtr(new StreamBuffer(data));
  }

  int64_t StreamBuffer::Read(unsigned char *target, int64_t bytes) {
    int64_t bytestoread = std::min(bytes, (int64_t)m_data.size() - m_index);
    std::memcpy(target, &m_data[(size_t)m_index], (size_t)bytestoread);
    m_index += bytestoread;
    return bytestoread;
  }
  bool StreamBuffer::Seek(int64_t offset) {
    if (offset < 0 || offset > (int64_t)m_data.size()) {
      return false;
    }
    m_index = offset;
    return true;
  }
  bool StreamBuffer::Seekable() const { return true; }

  StreamBuffer::StreamBuffer(const std::vector<unsigned char> &data) : m_data(data), m_index(0) { }
  StreamBuffer::~StreamBuffer() { }
}


