#ifndef FRAMES_EVENT
#define FRAMES_EVENT

#include "frames/detail.h"
#include "frames/noncopyable.h"
#include "frames/delegate.h"

#include "os_lua.h"

namespace Frames {
  class Layout;
  
  // Handle with member functions that can be called to get event data or modify event behavior
  class Handle : detail::Noncopyable {
  public:
    Handle(Layout *target) : m_target(target) { }
    ~Handle() { } // TODO - clean up handles in Lua?
    
    Layout *GetTarget() const { return m_target; }
    
    void luaF_push(lua_State *L);
    
  private:
    Layout *m_target;
  };
  
  namespace detail {
    // Base class for polymorphism
    class VerbBase : detail::Noncopyable {
    public:
      VerbBase(const char *name) : m_name(name), m_dive(0), m_bubble(0) { };
      VerbBase(const char *name, const VerbBase *dive, const VerbBase *bubble) : m_name(name), m_dive(dive), m_bubble(bubble) { };
    
      const char *GetName() const { return m_name; }
    
      const VerbBase *GetDive() const { return m_dive; }
      const VerbBase *GetBubble() const { return m_bubble; }
    
    private:
      const char *m_name;
      const VerbBase *m_dive;
      const VerbBase *m_bubble;
    };
  }
  
  template <typename Parameters> class VerbPackage;

  // Exists just to add templates
  template <typename Parameters> class Verb : public detail::VerbBase {
  private:
    friend class VerbPackage<Parameters>;
    Verb(const char *name, const Verb<Parameters> *dive, const Verb<Parameters> *bubble) : detail::VerbBase(name, dive, bubble) { };

  public:
    Verb(const char *name) : detail::VerbBase(name) { };
    
    typedef typename detail::FunctionPrefix<Handle*, Parameters>::T TypeHandler;
    typedef Delegate<typename detail::FunctionPrefix<Handle*, Parameters>::T> TypeDelegate;
  };

  template <typename Parameters> class VerbPackage : public Verb<Parameters> {
  public:
    VerbPackage(const char *name, const char *nameDive, const char *nameBubble) : Verb<Parameters>(name, &Dive, &Bubble), Dive(nameDive), Bubble(nameBubble) { };

    Verb<Parameters> Dive;
    Verb<Parameters> Bubble;
  };

  #define FRAMES_Verb_DECLARE_BEGIN \
    class Event { \
      Event();  /* intentionally left undefined */ \
      ~Event(); \
    public:

  #define FRAMES_Verb_DECLARE(eventname, paramlist) \
    /** \brief placeholder */ static Verb<void paramlist> eventname;

  #define FRAMES_Verb_DECLARE_BUBBLE(eventname, paramlist) \
    /** \brief placeholder */ static VerbPackage<void paramlist> eventname;

  #define FRAMES_Verb_DECLARE_END \
    };
}

#endif
