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

#include "FramesPCH.h"

/////////////////////
//
//   RETURN FROM WHENCE YOU CAME
//   NOTHING BUT SORROW AWAITS YOU IN THESE LANDS
//
/////////////////////

// This is awful.
// We do it because there is otherwise almost no way to access the HUD's canvas; we'd have to ask people to inherit from HUD and provide a function.
// But, given that they *could* do this, we don't feel too bad about forcing access to that canvas.
// Still.
// Eww.
// BE WARNED: This can cause all variety of weird linker errors. It appears that some functions have the access specifier as part of the signature. Weird, huh? I have no idea why. Maybe there's a reason!
// Anyway, variables thankfully don't, so it's safe *for this one use* as long as you never let this particular ghastliness out of its metaphorical padded cage.
// When I say "safe", you should be mentally adding air quotes around it.
#define protected public
#include "Engine.h"
#undef protected

UCanvas *FramesHUDHack(AHUD *hud)
{
  return hud->Canvas;
}
