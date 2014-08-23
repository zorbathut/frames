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

#include "FramesRect.generated.h"

USTRUCT(BlueprintType)
struct FRAMES_API FFramesRect {
  GENERATED_USTRUCT_BODY()

  FFramesRect() { } // no initialization
  FFramesRect(const FVector2D &Start, const FVector2D &End) : Start(Start), End(End) { }
  FFramesRect(float StartX, float StartY, float EndX, float EndY) : Start(StartX, StartY), End(EndX, EndY) { }

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frames|Layout")
  FVector2D Start;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Frames|Layout")
  FVector2D End;
};
