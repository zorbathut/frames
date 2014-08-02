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

#include "frames/layout.h"
#include "frames/layout_template_inline.h"

// File currently exists for the sole purpose of ensuring that its associated header builds cleanly.
// This one is a weird exception because this header is never meant to build on its own; we "bypass" its checks by including the minimum number of headers needed to make it build without intentional errors.
// Of course this is all ridiculous because layout.h actually includes layout_template_inline.h on its own.
// But, hey, that's what tests are for - if we didn't expect them to pass but want to know when they failed, we wouldn't bother writing them, would we?
