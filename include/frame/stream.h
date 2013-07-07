
// Input streams

#ifndef FRAME_STREAM
#define FRAME_STREAM

#include "frame/noncopyable.h"

#include <string>
#include <cstdio>

namespace Frame {
  class Stream : Noncopyable {
  public:
    Stream() { };
    virtual ~Stream() { };

    virtual int Read(unsigned char *target, int bytes) = 0;  // returns number of bytes, 0 indicates EOF
    virtual bool Seek(int offset) = 0;  // true if successful
    virtual bool Seekable() const = 0; // true if Seek is supported
  };

  class StreamFile : public Stream {
  public:
    static StreamFile *Create(const std::string &fname);

    virtual int Read(unsigned char *target, int bytes);
    virtual bool Seek(int offset);
    virtual bool Seekable() const;

  private:
    StreamFile(std::FILE *file);
    ~StreamFile();

    std::FILE *m_file;
  };
}

#endif

