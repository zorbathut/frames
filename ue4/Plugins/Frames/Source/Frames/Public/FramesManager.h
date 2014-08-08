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

#include "FramesLayout.h"
#include <frames/layout.h>

#include <map>

class FramesManager
{
public:
  // Retrieve singleton
  static FramesManager &Get();

  UFramesLayout *Convert(Frames::Layout *layout);
  UFramesEnvironment *Convert(Frames::Environment *env);

private:
  friend UFramesEnvironment;
  friend UFramesLayout;

  FramesManager();
  ~FramesManager();

  void DestroyFrameCallback(Frames::Handle *handle);
  void DestroyLayout(UFramesLayout *layout);
  void DestroyEnvironment(UFramesEnvironment *uenv);
  void RegisterEnvironment(UFramesEnvironment *uenv, Frames::Environment *env);

  // we intentionally don't register this as anything the GC understands - we want for it to not contribute to garbage collection
  std::map<Frames::Layout *, UFramesLayout *> m_mapLayout;
  std::map<UFramesLayout *, Frames::Layout *> m_mapLayoutReverse;
  std::map<Frames::Environment *, UFramesEnvironment *> m_mapEnvironment;
  std::map<UFramesEnvironment *, Frames::Environment *> m_mapEnvironmentReverse;

  // actual creation of appropriate type. TODO: some sort of registry for user-defined types?
  static UFramesLayout *Create(Frames::Layout *layout);
};
