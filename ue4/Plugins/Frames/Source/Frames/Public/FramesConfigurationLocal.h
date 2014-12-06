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

#include "FramesFont.h"

#include "FramesConfigurationLocal.generated.h"

/**
 * Frames configuration.
 */
UCLASS(Transient, BlueprintType, NotPlaceable)
class FRAMES_API UFramesConfigurationLocal : public UObject {
public:
  GENERATED_UCLASS_BODY()

  // Must be called first!
  UFUNCTION(BlueprintCallable, Category="Configuration", Meta=(DefaultToSelf="Context"))
  void Initialize(AActor *Context);

  UFUNCTION(BlueprintCallable, Category="Configuration")
  void FontDefaultIdSet(UFramesFont *Font);

  UFUNCTION(BlueprintCallable, Category="Configuration")
  UFramesFont *FontDefaultIdGet() const;

  // Available for C++-side users
  Frames::Configuration::Local m_config;
};
