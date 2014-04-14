
#include "frames/const.h"

#include "frames/detail.h"

namespace Frames {
  const float Nil = detail::Reinterpret<float>(0xFFF00DFF);
}
