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

#ifndef FRAMES_LAYOUT_TEMPLATE_INLINE
#define FRAMES_LAYOUT_TEMPLATE_INLINE

#include "frames/environment.h"

#ifndef FRAMES_LAYOUT
#error Do not include layout_template_inline.h independently!
#endif

namespace Frames {
  template <typename Parameters> void Layout::EventAttach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority /*= 0.0*/) {
    m_events[&event].insert(Callback::CreateNative(handler, priority));
  }
    
  template <typename Parameters> void Layout::EventDetach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority /*= detail::Undefined*/) {
    if (!m_events.count(&event)) {
      return;
    }
    
    std::multiset<Callback, Callback::Sorter> &eventSet = m_events.find(&event)->second;
    
    // TODO: Make this faster if it ever becomes a bottleneck!
    for (std::multiset<Callback, Callback::Sorter>::iterator itr = eventSet.begin(); itr != eventSet.end(); ++itr) {
      if (!itr->DestroyFlagGet() && itr->NativeCallbackEqual(handler) && (detail::IsUndefined(priority) || itr->PriorityGet() == priority)) {
        EventDestroy(m_events.find(&event), itr);
        return;
      }
    }
  }
    
  inline void Layout::EventTrigger(const Verb<void ()> &event) { // static is just so I can keep it in this file
    if (!m_events.count(&event)) {
      return;
    }

    m_env->ObliterateLock();
    
    Handle eh(this, &event);
    
    for (CallbackIterator itr = CallbackIterator(this, &event); !itr.Complete(); itr.Next()) {
      itr.Setup(&eh);
      itr.Get().Call(&eh);
    }

    m_env->ObliterateUnlock();
  }
  
  template <typename P1> void Layout::EventTrigger(const Verb<void (P1)> &event, typename detail::MakeConstRef<P1>::T p1) {
    if (!m_events.count(&event)) {
      return;
    }
    
    m_env->ObliterateLock();

    Handle eh(this, &event);
        
    for (CallbackIterator itr = CallbackIterator(this, &event); !itr.Complete(); itr.Next()) {
      itr.Setup(&eh);
      itr.Get().Call<P1>(&eh, p1);
    }

    m_env->ObliterateUnlock();
  }
}

#endif
