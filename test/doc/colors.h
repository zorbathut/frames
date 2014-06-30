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

#ifndef FRAMES_TEST_DOC_COLORS
#define FRAMES_TEST_DOC_COLORS

#include <frames/color.h>

namespace tdc {
  const Frames::Color red(0.3f, 0, 0, 1);
  const Frames::Color green(0, 0.3f, 0, 1);
  const Frames::Color blue(0, 0, 0.3f, 1);

  const Frames::Color redDark(0.15f, 0, 0, 1);
  const Frames::Color greenDark(0, 0.15f, 0, 1);
  const Frames::Color blueDark(0, 0, 0.15f, 1);
  
  const Frames::Color orange(0.3f, 0.2f, 0, 1);
  const Frames::Color cyan(0, 0.3f, 0.2f, 1);
  const Frames::Color purple(0.2f, 0, 0.3f, 1);

  const Frames::Color white(0.3f, 0.3f, 0.3f, 1);
  const Frames::Color whiteBright(0.5f, 0.5f, 0.5f, 1);
}

#endif
