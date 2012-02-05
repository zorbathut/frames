
#include "frames/stream.h"

namespace Frames {
  StreamFile *StreamFile::Create(const std::string &fname) {
    std::FILE *fil = fopen(fname.c_str(), "rb");
    if (!fil) return 0;
    return new StreamFile(fil);
  }

  int StreamFile::Read(unsigned char *target, int bytes) { return fread(target, 1, bytes, m_file); }
  bool StreamFile::Seek(int offset) { return !fseek(m_file, offset, SEEK_SET); }
  bool StreamFile::Seekable() const { return true; }

  StreamFile::StreamFile(std::FILE *file) : m_file(file) { }
  StreamFile::~StreamFile() { fclose(m_file); }
}


