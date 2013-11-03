// Template functions for Layout

#ifndef FRAME_LAYOUT_TEMPLATE_INLINE
#define FRAME_LAYOUT_TEMPLATE_INLINE

#ifndef FRAME_LAYOUT
#error Do not include layout_template_inline.h independently!
#endif

namespace Frames {
  template <typename Parameters> void Layout::EventAttach(const EventType<Parameters> &event, typename EventType<Parameters>::TypeDelegate handler, float priority /*= 0.0*/) {
    m_events[&event].insert(FECallback::CreateNative(handler, priority));
    
    EventAttached(&event);
  }
    
  template <typename Parameters> void Layout::EventDetach(const EventType<Parameters> &event, typename EventType<Parameters>::TypeDelegate handler, float priority /*= Utility::Undefined*/) {
    if (!m_events.count(&event)) {
      return;
    }
    
    const std::multiset<FECallback, FECallback::Sorter> &eventSet = m_events.find(&event)->second;
    
    // TODO: Make this faster if it ever becomes a bottleneck!
    for (std::multiset<FECallback, FECallback::Sorter>::iterator itr = eventSet.begin(); itr != eventSet.end(); ++itr) {
      if (!itr->DestroyFlagGet() && itr->NativeCallbackEqual(handler) && (Utility::IsUndefined(priority) || itr->PriorityGet() == priority)) {
        EventDestroy(m_events.find(&event), itr);
        EventDetached(&event);
        return;
      }
    }
  }
    
  inline void Layout::EventTrigger(const EventType<void ()> &event) { // static is just so I can keep it in this file
    if (!m_events.count(&event)) {
      return;
    }
    
    EventHandle eh(this);
    
    for (FEIterator itr = FEIterator(this, &event); !itr.Complete(); itr.Next()) {
      itr.Get().Call(&eh);
    }
  }
  
  template <typename P1> void Layout::EventTrigger(const EventType<void (P1)> &event, typename Utility::MakeConstRef<P1>::T p1) {
    if (!m_events.count(&event)) {
      return;
    }
    
    EventHandle eh(this);
        
    for (FEIterator itr = FEIterator(this, &event); !itr.Complete(); itr.Next()) {
      itr.Get().Call<P1>(&eh, p1);
    }
  }
}

#endif
