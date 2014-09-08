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

#include "FramesEnvironment.h"
#include "FramesFrame.h"
#include "FramesText.h"
#include "FramesSprite.h"
#include "FramesMask.h"

#include <frames/cast.h>
#include <frames/mask.h>

  // Retrieve singleton
FramesManager &FramesManager::Get() {
  static FramesManager mgr;
  return mgr;
}

UFramesLayout *FramesManager::Convert(Frames::Layout *layout) {
  if (!layout) {
    return 0;
  }

  if (!m_mapLayout.count(layout)) {
    UFramesLayout *result = Create(layout);
    m_mapLayout[layout] = result;
    m_mapLayoutReverse[result] = layout;
    layout->EventAttach(Frames::Layout::Event::Destroy, Frames::Delegate<void (Frames::Handle *)>(this, &FramesManager::DestroyFrameCallback));
    return result;
  }

  return m_mapLayout[layout];
}

UFramesEnvironment *FramesManager::Convert(Frames::Environment *env) {
  if (!env) {
    return 0;
  }

  if (!m_mapEnvironment.count(env)) {
    UFramesEnvironment *uenv = new UFramesEnvironment(FPostConstructInitializeProperties());
    uenv->m_env = Frames::EnvironmentPtr(env);
    m_mapEnvironment[env] = uenv;
    m_mapEnvironmentReverse[uenv] = env;
    return uenv;
  }

  return m_mapEnvironment[env];
}

FramesManager::FramesManager() { }
FramesManager::~FramesManager() {
  for (std::map<Frames::Layout *, UFramesLayout *>::const_iterator itr = m_mapLayout.begin(); itr != m_mapLayout.end(); ++itr) {
    itr->first->EventDetach(Frames::Layout::Event::Destroy, Frames::Delegate<void (Frames::Handle *)>(this, &FramesManager::DestroyFrameCallback));
  }
  m_mapLayout.clear();
}

void FramesManager::DestroyFrameCallback(Frames::Handle *handle) {
  UFramesLayout *ul = m_mapLayout[handle->TargetGet()];
  if (ul) {
    ul->m_layout = 0; // clear it out
  }
  m_mapLayout.erase(handle->TargetGet());
  m_mapLayoutReverse.erase(ul);
}

void FramesManager::DestroyLayout(UFramesLayout *ul) {
  m_mapLayout.erase(m_mapLayoutReverse[ul]);
  m_mapLayoutReverse.erase(ul);
}

void FramesManager::DestroyEnvironment(UFramesEnvironment *uenv) {
  m_mapEnvironment.erase(m_mapEnvironmentReverse[uenv]);
  m_mapEnvironmentReverse.erase(uenv);
}

void FramesManager::RegisterEnvironment(UFramesEnvironment *uenv, Frames::Environment *env) {
  m_mapEnvironment[env] = uenv;
  m_mapEnvironmentReverse[uenv] = env;
}

/*static*/ UFramesLayout *FramesManager::Create(Frames::Layout *layout) {
  UFramesLayout *result = 0;
  if (Frames::Cast<Frames::Mask>(layout)) {
    result = new UFramesMask(FPostConstructInitializeProperties());
  } else if (Frames::Cast<Frames::Sprite>(layout)) {
    result = new UFramesSprite(FPostConstructInitializeProperties());
  } else if (Frames::Cast<Frames::Text>(layout)) {
    result = new UFramesText(FPostConstructInitializeProperties());
  } else if (Frames::Cast<Frames::Frame>(layout)) {
    result = new UFramesFrame(FPostConstructInitializeProperties());
  } else {
    result = new UFramesLayout(FPostConstructInitializeProperties());
  }
  result->m_layout = layout;
  return result;
}
