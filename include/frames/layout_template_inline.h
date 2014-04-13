// Template functions for Layout

#ifndef FRAMES_LAYOUT_TEMPLATE_INLINE
#define FRAMES_LAYOUT_TEMPLATE_INLINE

#ifndef FRAMES_LAYOUT
#error Do not include layout_template_inline.h independently!
#endif

namespace Frames {
  template <typename Parameters> void Layout::EventAttach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority /*= 0.0*/) {
    m_events[&event].insert(FECallback::CreateNative(handler, priority));
  }
    
  template <typename Parameters> void Layout::EventDetach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority /*= detail::Undefined*/) {
    if (!m_events.count(&event)) {
      return;
    }
    
    std::multiset<FECallback, FECallback::Sorter> &eventSet = m_events.find(&event)->second;
    
    // TODO: Make this faster if it ever becomes a bottleneck!
    for (std::multiset<FECallback, FECallback::Sorter>::iterator itr = eventSet.begin(); itr != eventSet.end(); ++itr) {
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
    
    Handle eh(this);
    
    for (FEIterator itr = FEIterator(this, &event); !itr.Complete(); itr.Next()) {
      itr.Get().Call(&eh);
    }
  }
  
  template <typename P1> void Layout::EventTrigger(const Verb<void (P1)> &event, typename detail::MakeConstRef<P1>::T p1) {
    if (!m_events.count(&event)) {
      return;
    }
    
    Handle eh(this);
        
    for (FEIterator itr = FEIterator(this, &event); !itr.Complete(); itr.Next()) {
      itr.Get().Call<P1>(&eh, p1);
    }
  }
}

#endif
