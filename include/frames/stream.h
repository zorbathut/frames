
// Input streams

#ifndef FRAMES_STREAM
#define FRAMES_STREAM

#include "frames/noncopyable.h"

#include <string>
#include <cstdio>

namespace Frames {
  class Stream : Noncopyable {
  public:
    Stream() { };
    virtual ~Stream() { };

    virtual int Read(char *target, int bytes) = 0;  // returns number of bytes, 0 indicates EOF
    virtual bool Seek(int offset) = 0;  // true if successful
    virtual bool Seekable() const = 0; // true if Seek is supported
  };

  class StreamFile : public Stream {
  public:
    static StreamFile *Create(const std::string &fname);

    virtual int Read(char *target, int bytes);
    virtual bool Seek(int offset);
    virtual bool Seekable() const;

  private:
    StreamFile(std::FILE *file);
    ~StreamFile();

    std::FILE *m_file;
  };
}

#endif

