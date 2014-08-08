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

#include "FramesPCH.h"

#include "FramesManager.h"

#include "FramesFrame.h"

  // Retrieve singleton
FramesManager &FramesManager::Get() {
  static FramesManager mgr;
  return mgr;
}

UFramesLayout *FramesManager::Convert(Frames::Layout *layout) {
  if (!layout) {
    return 0;
  }

  if (!m_map.count(layout)) {
    UFramesLayout *result = Create(layout);
    m_map[layout] = result;
    m_mapReverse[result] = layout;
    layout->EventAttach(Frames::Layout::Event::Destroy, Frames::Delegate<void (Frames::Handle *)>(this, &FramesManager::DestroyFrameCallback));
    return result;
  }

  return m_map[layout];
}

FramesManager::FramesManager() { }
FramesManager::~FramesManager() {
  for (std::map<Frames::Layout *, UFramesLayout *>::const_iterator itr = m_map.begin(); itr != m_map.end(); ++itr) {
    itr->first->EventDetach(Frames::Layout::Event::Destroy, Frames::Delegate<void (Frames::Handle *)>(this, &FramesManager::DestroyFrameCallback));
  }
  m_map.clear();
}

void FramesManager::DestroyFrameCallback(Frames::Handle *handle) {
  UFramesLayout *ul = m_map[handle->TargetGet()];
  if (ul) {
    ul->m_layout = 0; // clear it out
  }
  m_map.erase(handle->TargetGet());
  m_mapReverse.erase(ul);
}

void FramesManager::DestroyLayout(UFramesLayout *ul) {
  m_map.erase(m_mapReverse[ul]);
  m_mapReverse.erase(ul);
}

/*static*/ UFramesLayout *FramesManager::Create(Frames::Layout *layout) {
  UFramesLayout *result = 0;
  if (Frames::Cast<Frames::Frame>(layout)) {
    result = new UFramesFrame(FPostConstructInitializeProperties());
  } else {
    result = new UFramesLayout(FPostConstructInitializeProperties());
  }
  result->m_layout = layout;
  return result;
}
