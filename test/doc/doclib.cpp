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

#include "doclib.h"

Frames::Text *CreateNameBar(Frames::Layout *root, const std::string &header) {
  Frames::Text *text = Frames::Text::Create(root, "Header");

  text->TextSet(header);
  text->PinSet(Frames::CENTERTOP, root, Frames::CENTERTOP, 0, 30);
  text->SizeSet(30);
  text->ColorTextSet(Frames::Color(1.f, 1.f, 0.f));
  text->FontSet("geo_1.ttf");

  return text;
}
