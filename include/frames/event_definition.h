// Frame event definitions. This file needs to be included from any file that defines events.

#ifndef FRAMES_EVENT_DEFINITION
#define FRAMES_EVENT_DEFINITION

// not that this helps the defines, but whatever
namespace Frames {
  #define FRAMES_FRAMEEVENT_DEFINE_PARAMETER_PREFIX this, eids, eid, eidb

  #define FRAMES_FRAMEEVENT_DEFINE_INFRA(frametype, eventname, paramlist, paramlistcomplete, params) \
    void frametype::Event##eventname##Attach(Delegate<void paramlistcomplete> delegate, float order /*= 0.f*/) { \
      INTERNAL_EventAttach(Event##eventname##Id(), EventHandler(delegate), order); \
    } \
    void frametype::Event##eventname##Detach(Delegate<void paramlistcomplete> delegate, float order /*= 0.f / 0.f*/) { \
      INTERNAL_EventDetach(Event##eventname##Id(), EventHandler(delegate), order); \
    } \
    /*static*/ EventId frametype::Event##eventname##Id() { \
      return (EventId)&s_event_##eventname##_id; \
    } \
    char frametype::s_event_##eventname##_id = 0;

  template<typename F> struct INTERNAL_EventHandlerCaller;
  template<> struct INTERNAL_EventHandlerCaller<void ()> { static void Call(const Layout::EventHandler *ev, EventHandle *eh) { ev->Call<EventHandle *>(eh); } };
  template<typename P1> struct INTERNAL_EventHandlerCaller<void (P1)> { static void Call(const Layout::EventHandler *ev, EventHandle *eh, P1 p1) { ev->Call<EventHandle *, P1>(eh, p1); } };
  template<typename P1, typename P2> struct INTERNAL_EventHandlerCaller<void (P1, P2)> { static void Call(const Layout::EventHandler *ev, EventHandle *eh, P1 p1, P2 p2) { ev->Call<EventHandle *, P1, P2>(eh, p1, p2); } };
  template<typename P1, typename P2, typename P3> struct INTERNAL_EventHandlerCaller<void (P1, P2, P3)> { static void Call(const Layout::EventHandler *ev, EventHandle *eh, P1 p1, P2 p2, P3 p3) { ev->Call<EventHandle *, P1, P2, P3>(eh, p1, p2, p3); } };
  template<typename P1, typename P2, typename P3, typename P4> struct INTERNAL_EventHandlerCaller<void (P1, P2, P3, P4)> { static void Call(const Layout::EventHandler *ev, EventHandle *eh, P1 p1, P2 p2, P3 p3, P4 p4) { ev->Call<EventHandle *, P1, P2, P3, P4>(eh, p1, p2, p3, p4); } };

  #define FRAMES_INTERNAL_EVENTDISPATCH_BODY_PARAMLIST_PREFIX &ev->second, &handle
  #define FRAMES_INTERNAL_EVENTDISPATCH_BODY(paramlist) \
      std::map<EventId, std::multimap<float, Layout::EventHandler> >::const_iterator itr = layout->m_events.find(eid); \
      if (itr != layout->m_events.end()) { \
        EventHandle handle = EventHandle::INTERNAL_Initialize(layout); \
        handle.INTERNAL_SetCanFinalize(true); \
        bool oboldlock = layout->Obliterate_Lock(); \
        bool evoldlock = layout->Event_Lock(); \
        const std::multimap<float, Layout::EventHandler> &tab = itr->second; \
        for (std::multimap<float, Layout::EventHandler>::const_iterator ev = tab.begin(); ev != tab.end() && !handle.GetFinalize(); ++ev) { \
          INTERNAL_EventHandlerCaller<Type>::Call paramlist; /* starts with &ev->second, &handle */ \
        } \
        layout->Event_Unlock(evoldlock); \
        layout->Obliterate_Unlock(oboldlock); \
      }

  template<typename F> struct INTERNAL_EventDispatch;
  template<> struct INTERNAL_EventDispatch<void ()> { typedef void Type(); static void Call(Layout *layout, int eids, int eid, int eidb) {
    FRAMES_INTERNAL_EVENTDISPATCH_BODY((FRAMES_INTERNAL_EVENTDISPATCH_BODY_PARAMLIST_PREFIX));
  }};
  template<typename P1> struct INTERNAL_EventDispatch<void (P1)> { typedef void Type(P1); static void Call(Layout *layout, int eids, int eid, int eidb, P1 p1) {
    FRAMES_INTERNAL_EVENTDISPATCH_BODY((FRAMES_INTERNAL_EVENTDISPATCH_BODY_PARAMLIST_PREFIX, p1));
  }};
  template<typename P1, typename P2> struct INTERNAL_EventDispatch<void (P1, P2)> { typedef void Type(P1, P2); static void Call(Layout *layout, int eids, int eid, int eidb, P1 p1, P2 p2) {
    FRAMES_INTERNAL_EVENTDISPATCH_BODY((FRAMES_INTERNAL_EVENTDISPATCH_BODY_PARAMLIST_PREFIX, p1, p2));
  }};
  template<typename P1, typename P2, typename P3> struct INTERNAL_EventDispatch<void (P1, P2, P3)> { typedef void Type(P1, P2, P3); static void Call(Layout *layout, int eids, int eid, int eidb, P1 p1, P2 p2, P3 p3) {
    FRAMES_INTERNAL_EVENTDISPATCH_BODY((FRAMES_INTERNAL_EVENTDISPATCH_BODY_PARAMLIST_PREFIX, p1, p2, p3));
  }};
  template<typename P1, typename P2, typename P3, typename P4> struct INTERNAL_EventDispatch<void (P1, P2, P3, P4)> { typedef void Type(P1, P2, P3, P4); static void Call(Layout *layout, int eids, int eid, int eidb, P1 p1, P2 p2, P3 p3, P4 p4) {
    FRAMES_INTERNAL_EVENTDISPATCH_BODY((FRAMES_INTERNAL_EVENTDISPATCH_BODY_PARAMLIST_PREFIX, p1, p2, p3, p4));
  }};
  #undef FRAMES_INTERNAL_EVENTDISPATCH_BODY
  #undef FRAMES_INTERNAL_EVENTDISPATCH_BODY_PARAMLIST_PREFIX

  #define FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY_PARAMLIST_PREFIX &ev->second, &handle
  #define FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY(paramlist) \
      bool oboldlock = layout->Obliterate_Lock(); \
      std::vector<std::pair<Layout *, bool> > layouts; \
      Layout *parent = layout->GetParent(); \
      while (parent) { \
        layouts.push_back(std::make_pair(parent, parent->Obliterate_Lock())); \
        parent = parent->GetParent(); \
      } \
      EventHandle handle = EventHandle::INTERNAL_Initialize(layout); \
      for (int i = (int)layouts.size() - 1; i >= 0; --i) { \
        std::map<EventId, std::multimap<float, Layout::EventHandler> >::const_iterator itr = layouts[i].first->m_events.find(eids); \
        if (itr != layouts[i].first->m_events.end()) { \
          bool evoldlock = layouts[i].first->Event_Lock(); \
          const std::multimap<float, Layout::EventHandler> &tab = itr->second; \
          for (std::multimap<float, Layout::EventHandler>::const_iterator ev = tab.begin(); ev != tab.end(); ++ev) { \
            INTERNAL_EventHandlerCaller<Type>::Call paramlist; \
          } \
          layouts[i].first->Event_Unlock(evoldlock); \
        } \
      } \
      { \
        std::map<EventId, std::multimap<float, Layout::EventHandler> >::const_iterator itr = layout->m_events.find(eid); \
        if (itr != layout->m_events.end()) { \
          handle.INTERNAL_SetCanFinalize(true); \
          bool evoldlock = layout->Event_Lock(); \
          const std::multimap<float, Layout::EventHandler> &tab = itr->second; \
          for (std::multimap<float, Layout::EventHandler>::const_iterator ev = tab.begin(); ev != tab.end() && !handle.GetFinalize(); ++ev) { \
            INTERNAL_EventHandlerCaller<Type>::Call paramlist; \
          } \
          layout->Event_Unlock(evoldlock); \
          handle.Finalize(); \
          handle.INTERNAL_SetCanFinalize(false); \
        } \
      } \
      for (int i = 0; i < (int)layouts.size(); ++i) { \
        std::map<EventId, std::multimap<float, Layout::EventHandler> >::const_iterator itr = layouts[i].first->m_events.find(eidb); \
        if (itr != layouts[i].first->m_events.end()) { \
          bool evoldlock = layouts[i].first->Event_Lock(); \
          const std::multimap<float, Layout::EventHandler> &tab = itr->second; \
          for (std::multimap<float, Layout::EventHandler>::const_iterator ev = tab.begin(); ev != tab.end(); ++ev) { \
            INTERNAL_EventHandlerCaller<Type>::Call paramlist; \
          } \
          layouts[i].first->Event_Unlock(evoldlock); \
        } \
      } \
      for (int i = 0; i < (int)layouts.size(); ++i) { \
        layouts[i].first->Obliterate_Unlock(layouts[i].second); \
      } \
      layout->Obliterate_Unlock(oboldlock); \

  template<typename F> struct INTERNAL_EventDispatchBubble;
  template<> struct INTERNAL_EventDispatchBubble<void ()> { typedef void Type(); static void Call(Layout *layout, int eids, int eid, int eidb) {
    FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY((FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY_PARAMLIST_PREFIX));
  }};
  template<typename P1> struct INTERNAL_EventDispatchBubble<void (P1)> { typedef void Type(P1); static void Call(Layout *layout, int eids, int eid, int eidb, P1 p1) {
    FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY((FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY_PARAMLIST_PREFIX, p1));
  }};
  template<typename P1, typename P2> struct INTERNAL_EventDispatchBubble<void (P1, P2)> { typedef void Type(P1, P2); static void Call(Layout *layout, int eids, int eid, int eidb, P1 p1, P2 p2) {
    FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY((FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY_PARAMLIST_PREFIX, p1, p2));
  }};
  template<typename P1, typename P2, typename P3> struct INTERNAL_EventDispatchBubble<void (P1, P2, P3)> { typedef void Type(P1, P2, P3); static void Call(Layout *layout, int eids, int eid, int eidb, P1 p1, P2 p2, P3 p3) {
    FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY((FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY_PARAMLIST_PREFIX, p1, p2, p3));
  }};
  template<typename P1, typename P2, typename P3, typename P4> struct INTERNAL_EventDispatchBubble<void (P1, P2, P3, P4)> { typedef void Type(P1, P2, P3, P4); static void Call(Layout *layout, int eids, int eid, int eidb, P1 p1, P2 p2, P3 p3, P4 p4) {
    FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY((FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY_PARAMLIST_PREFIX, p1, p2, p3, p4));
  }};
  #undef FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY
  #undef FRAMES_INTERNAL_EVENTDISPATCHBUBBLE_BODY_PARAMLIST_PREFIX

  #define FRAMES_FRAMEEVENT_DEFINE(frametype, eventname, paramlist, paramlistcomplete, params) \
    FRAMES_FRAMEEVENT_DEFINE_INFRA(frametype, eventname, paramlist, paramlistcomplete, params) \
    void frametype::Event##eventname##Trigger paramlist { \
      int eids = 0; \
      int eid = Event##eventname##Id(); \
      int eidb = 0; \
      INTERNAL_EventDispatch<Event##eventname##Functiontype>::Call params; /* includes "this, eids, eid, eidb" at the beginning */ \
    }

  #define FRAMES_FRAMEEVENT_DEFINE_BUBBLE(frametype, eventname, paramlist, paramlistcomplete, params) \
    FRAMES_FRAMEEVENT_DEFINE_INFRA(frametype, eventname, paramlist, paramlistcomplete, params) \
    FRAMES_FRAMEEVENT_DEFINE_INFRA(frametype, eventname##Sink, paramlist, paramlistcomplete, params) \
    FRAMES_FRAMEEVENT_DEFINE_INFRA(frametype, eventname##Bubble, paramlist, paramlistcomplete, params) \
    void frametype::Event##eventname##Trigger paramlist { \
      int eids = Event##eventname##SinkId(); \
      int eid = Event##eventname##Id(); \
      int eidb = Event##eventname##BubbleId(); \
      INTERNAL_EventDispatchBubble<Event##eventname##Functiontype>::Call params; /* includes "this, eids, eid, eidb" at the beginning */ \
    }

  #define FRAMES_FRAMEEVENT_L_REGISTER(L, GST, name, handler) \
    l_RegisterEvent<Event##name##Functiontype>(L, GST, "Event" #name "Attach", "Event" #name "Detach", Event##name##Id(), static_cast<typename LayoutHandlerMunger<Event##name##Functiontype>::T>(handler));

  #define FRAMES_FRAMEEVENT_L_REGISTER_BUBBLE(L, GST, name, handler) \
    FRAMES_FRAMEEVENT_L_REGISTER(L, GST, name, handler); \
    FRAMES_FRAMEEVENT_L_REGISTER(L, GST, name##Sink, handler); \
    FRAMES_FRAMEEVENT_L_REGISTER(L, GST, name##Bubble, handler);
}

#endif
