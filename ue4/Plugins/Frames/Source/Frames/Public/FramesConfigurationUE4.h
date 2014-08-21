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

#ifndef FRAMES_UE4_CONFIGURATION_UE4
#define FRAMES_UE4_CONFIGURATION_UE4

#include <frames/configuration.h>

namespace Frames {
  namespace Configuration {
    /// Creates a Frames::Configuration::Local with configuration elements initialized to the recommended UE4-integration settings.
    Frames::Configuration::Local CreateUE4();

    /// Standard logger module for UE4.
    /** errorScreen controls whether it displays errors on the screen. errorAssert controls whether it asserts on errors. */
    class LoggerUE4 : Frames::Configuration::Logger {
    public:
      LoggerUE4(bool errorScreen, bool errorAssert);

      virtual void LogError(const std::string &log) override;
      virtual void LogDebug(const std::string &log) override;

    private:
      bool m_onScreenErrors;
      bool m_assertErrors;
    };

    /// Standard texture reader module for UE4.
    /** Reads textures from the UE4 assets. Usable only with RendererRHI. */
    class TextureFromIdUE4 : public Frames::Configuration::TextureFromId {
    public:
      virtual Frames::TexturePtr Create(Frames::Environment *env, const std::string &id) override;
    };

    /// Standard stream reader module for UE4.
    /** This currently is intended only for use with fonts, and will be redesigned when a better font solution is available. */
    class StreamFromIdUE4 : public Frames::Configuration::StreamFromId {
    public:
      virtual Frames::StreamPtr Create(Frames::Environment *env, const std::string &id) override;
    };
  }
}

#endif
