
#include "frames/vector.h"

namespace Frames {
  std::ostream &operator<<(std::ostream &str, const Vector &rhs) {
    return str << "(" << rhs.x << ", " << rhs.y << ")";
  }
}

