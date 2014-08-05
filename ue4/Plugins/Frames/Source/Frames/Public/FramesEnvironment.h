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

#include <frames/environment.h>
#include <frames/configuration.h>
#include <frames/renderer_null.h>

#include "Engine.h"

#include "FramesEnvironment.generated.h"

/**
 * Frames main environment.
 */
UCLASS(Transient, Blueprintable, BlueprintType, NotPlaceable)
class UFramesEnvironment : public UObject
{
  GENERATED_UCLASS_BODY()

public:
  UFUNCTION(BlueprintCallable, Category="Frames|Rendering")
  void Render(AHUD *hud);

  UFUNCTION(BlueprintCallable, Category="Frames|Development")
  void LogDebug(const FString &text) const;

  UFUNCTION(BlueprintCallable, Category="Frames|Development")
  void LogError(const FString &text) const;

  // Provided for C++ users.
  void Initialize(const Frames::Configuration::Local &config);
  const Frames::EnvironmentPtr &EnvironmentGet();

private:
  Frames::EnvironmentPtr m_env;
};
