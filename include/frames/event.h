#ifndef FRAMES_EVENT
#define FRAMES_EVENT

#include "frames/detail.h"
#include "frames/noncopyable.h"
#include "frames/delegate.h"

#include "os_lua.h"

namespace Frames {
  class Layout;
  
  // Handle with member functions that can be called to get event data or modify event behavior
  class EventHandle : detail::Noncopyable {
  public:
    EventHandle(Layout *target) : m_target(target) { }
    ~EventHandle() { } // TODO - clean up handles in Lua?
    
    Layout *GetTarget() const { return m_target; }
    
    void luaF_push(lua_State *L);
    
  private:
    Layout *m_target;
  };
  
  // Base class for polymorphism
  class EventBase : detail::Noncopyable {
  public:
    EventBase(const char *name) : m_name(name), m_dive(0), m_bubble(0) { };
    EventBase(const char *name, const EventBase *dive, const EventBase *bubble) : m_name(name), m_dive(dive), m_bubble(bubble) { };
    
    const char *GetName() const { return m_name; }
    
    const EventBase *GetDive() const { return m_dive; }
    const EventBase *GetBubble() const { return m_bubble; }
    
  private:
    const char *m_name;
    const EventBase *m_dive;
    const EventBase *m_bubble;
  };
  
  // Exists just to add templates
  template <typename Parameters> class EventType : public EventBase {
  public:
    EventType(const char *name) : EventBase(name) { };
    EventType(const char *name, const EventType<Parameters> *dive, const EventType<Parameters> *bubble) : EventBase(name, dive, bubble) { };
    
    typedef typename detail::FunctionPrefix<EventHandle*, Parameters>::T TypeHandler;
    typedef Delegate<typename detail::FunctionPrefix<EventHandle*, Parameters>::T> TypeDelegate;
  };

  #define FRAMES_FRAMEEVENT_DECLARE_BEGIN \
    class Event { \
      Event();  /* intentionally left undefined */ \
      ~Event(); \
    public:

  #define FRAMES_FRAMEEVENT_DECLARE(eventname, paramlist) \
    static EventType<void paramlist> eventname;

  #define FRAMES_FRAMEEVENT_DECLARE_BUBBLE(eventname, paramlist) \
    static EventType<void paramlist> eventname; \
    static EventType<void paramlist> eventname##Dive; \
    static EventType<void paramlist> eventname##Bubble;

  #define FRAMES_FRAMEEVENT_DECLARE_END \
    };
}

#endif
