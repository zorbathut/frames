#ifndef FRAME_EVENT
#define FRAME_EVENT

#include "frame/utility.h"
#include "frame/noncopyable.h"
#include "frame/delegate.h"

#include "os_lua.h"

namespace Frame {
  class Layout;
  
  // Handle with member functions that can be called to get event data or modify event behavior
  class EventHandle : Noncopyable {
  public:
    EventHandle(Layout *target) : m_target(target) { }
    ~EventHandle() { } // TODO - clean up handles in Lua?
    
    Layout *GetTarget() const { return m_target; }
    
    void luaF_push(lua_State *L);
    
  private:
    Layout *m_target;
  };
  
  // Base class for polymorphism
  class EventTypeBase : Noncopyable {
  public:
    EventTypeBase(const char *name) : m_name(name), m_dive(0), m_bubble(0) { };
    EventTypeBase(const char *name, const EventTypeBase *dive, const EventTypeBase *bubble) : m_name(name), m_dive(dive), m_bubble(bubble) { };
    
    const char *GetName() const { return m_name; }
    
    const EventTypeBase *GetDive() const { return m_dive; }
    const EventTypeBase *GetBubble() const { return m_bubble; }
    
  private:
    const char *m_name;
    const EventTypeBase *m_dive;
    const EventTypeBase *m_bubble;
  };
  
  // Exists just to add templates - all templates must be specialized on Delegate!
  template <typename Parameters> class EventType : public EventTypeBase {
  public:
    EventType(const char *name) : EventTypeBase(name) { };
    EventType(const char *name, const EventType<Parameters> *dive, const EventType<Parameters> *bubble) : EventTypeBase(name, dive, bubble) { };
    
    typedef typename Utility::FunctionPrefix<EventHandle*, Parameters>::T TypeHandler;
    typedef Delegate<typename Utility::FunctionPrefix<EventHandle*, Parameters>::T> TypeDelegate;
  };

  #define FRAME_FRAMEEVENT_DECLARE(eventname, paramlist) \
    namespace Event { \
      extern EventType<void paramlist> eventname; \
    }

  #define FRAME_FRAMEEVENT_DECLARE_BUBBLE(eventname, paramlist) \
    namespace Event { \
      extern EventType<void paramlist> eventname; \
      extern EventType<void paramlist> eventname##Dive; \
      extern EventType<void paramlist> eventname##Bubble; \
    }
}

#endif
