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

#include <frames/configuration.h>
#include <frames/layout.h>

#include "Engine.h"

#include "FramesLayout.generated.h"

UCLASS(Transient, BlueprintType, NotPlaceable)
class UFramesLayout : public UObject
{
  GENERATED_UCLASS_BODY()
  ~UFramesLayout();

  Frames::Layout *LayoutGet() const { return m_layout; }

  // Checks to see if a layout is valid; outputs an error and returns false if it's not
  bool ValidCheck() const;

private:
  friend class FramesManager;

  Frames::Layout *m_layout;
};
