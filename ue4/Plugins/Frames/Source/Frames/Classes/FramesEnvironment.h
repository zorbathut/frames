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

#include "AllowWindowsPlatformTypes.h"
#include <frames/environment.h>
#include <frames/configuration.h>
#include <frames/renderer_null.h>
#include "HideWindowsPlatformTypes.h"

#include "FramesEnvironment.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(FramesLog, Log, All);

/**
 * Frames main environment.
 */
UCLASS(Transient, Blueprintable, BlueprintType, NotPlaceable)
class UFramesEnvironment : public UObject
{
	GENERATED_UCLASS_BODY()

public:
  UFUNCTION(BlueprintCallable, Category="Render")
  void Render(AHUD *hud);

private:
  Frames::EnvironmentPtr m_env;
};
