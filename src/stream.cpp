
#include "frames/stream.h"

#include <algorithm>
#include <cstring>

namespace Frames {
  StreamFile *StreamFile::Create(const std::string &fname) {
    std::FILE *fil = fopen(fname.c_str(), "rb");
    if (!fil) return 0;
    return new StreamFile(fil);
  }

  int StreamFile::Read(unsigned char *target, int bytes) { return (int)fread(target, 1, bytes, m_file); } // safe to cast because our input is "int" anyway, so it can't possibly read more than that
  bool StreamFile::Seek(int offset) { return !fseek(m_file, offset, SEEK_SET); }
  bool StreamFile::Seekable() const { return true; }

  StreamFile::StreamFile(std::FILE *file) : m_file(file) { }
  StreamFile::~StreamFile() { fclose(m_file); }
  
  StreamBuffer *StreamBuffer::Create(const std::vector<unsigned char> &data) {
    return new StreamBuffer(data);
  }

  int StreamBuffer::Read(unsigned char *target, int bytes) {
    int bytestoread = std::min(bytes, (int)m_data.size() - m_index);
    std::memcpy(target, &m_data[m_index], bytestoread);
    m_index += bytestoread;
    return bytestoread;
  }
  bool StreamBuffer::Seek(int offset) {
    if (offset < 0 || offset > (int)m_data.size()) {
      return false;
    }
    m_index = offset;
    return true;
  }
  bool StreamBuffer::Seekable() const { return true; }

  StreamBuffer::StreamBuffer(const std::vector<unsigned char> &data) : m_data(data), m_index(0) { }
  StreamBuffer::~StreamBuffer() { }
}


