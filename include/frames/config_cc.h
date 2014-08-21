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

#ifndef FRAMES_CONFIG_CC
#define FRAMES_CONFIG_CC

#if defined(__GNUC__)
  #define FRAMES_EXPECT(expression, expected) __builtin_expect((expression), (expected))
#elif defined(_MSC_VER)
  #define FRAMES_EXPECT(expression, expected) (expression)
#else
  #warning Unknown compiler, falling back to no branch hinting
  #define FRAMES_EXPECT(expression, expected) (expression)
#endif

#if defined(_MSC_VER) && _MSC_VER == 1500
  // not supported on msvc9
  #define FRAMES_OVERRIDE
#else
  // supported; non-standard on msvc10, but at least well enough to use. standard everywhere else
  #define FRAMES_OVERRIDE override
#endif

#endif
