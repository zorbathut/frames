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

#pragma once

#include "CoreUObject.h"

// This is awful.
// We do it because there is otherwise almost no way to access the HUD's canvas; we'd have to ask people to inherit from HUD and provide a function.
// But, given that they *could* do this, we don't feel too bad about forcing access to that canvas.
// Still.
// Eww.
#define protected public
#include "Engine.h"
#undef protected
