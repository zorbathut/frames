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

#ifndef FRAMES_UE4_CONVERT
#define FRAMES_UE4_CONVERT

#include <frames/vector.h>
#include <frames/rect.h>

#include "FramesRect.h"

namespace Frames {
  namespace detail {
    static std::string UE4Convert(const FString &fstr) {
      return TCHAR_TO_UTF8(*fstr);
    }

    static FString UE4Convert(const std::string &utfstr) {
      return UTF8_TO_TCHAR(utfstr.c_str());
    }

    static FVector2D UE4Convert(const Frames::Vector &vec) {
      return FVector2D(vec.x, vec.y);
    }

    static Frames::Vector UE4Convert(const FVector2D &vec) {
      return Frames::Vector(vec.X, vec.Y);
    }

    static FFramesRect UE4Convert(const Frames::Rect &rect) {
      return FFramesRect(UE4Convert(rect.s), UE4Convert(rect.e));
    }

    static Frames::Rect UE4Convert(const FFramesRect &rect) {
      return Frames::Rect(UE4Convert(rect.Start), UE4Convert(rect.End));
    }
  }
}

#endif
