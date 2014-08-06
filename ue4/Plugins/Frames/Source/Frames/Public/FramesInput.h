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
    along with Frames.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <frames/input.h>

#include "FramesInput.generated.h"

USTRUCT(BlueprintType)
struct FFramesInputMeta
{
  GENERATED_USTRUCT_BODY()

  FFramesInputMeta() : Shift(false), Ctrl(false), Alt(false) { }
  FFramesInputMeta(Frames::Input::Meta meta) : Shift(meta.shift), Ctrl(meta.ctrl), Alt(meta.alt) { }

  Frames::Input::Meta ToFrames() const {
    Frames::Input::Meta rv;
    rv.shift = Shift;
    rv.ctrl = Ctrl;
    rv.alt = Alt;
    return rv;
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frames|Input")
  bool Shift;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frames|Input")
  bool Ctrl;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frames|Input")
  bool Alt;
};
