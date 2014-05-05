#ifndef FRAMES_EVENT
#define FRAMES_EVENT

#include "frames/detail.h"
#include "frames/noncopyable.h"
#include "frames/delegate.h"

namespace Frames {
  class Layout;
  
  template <typename Parameters> class VerbPackage;

  /// Base class for type-specific Verbs.
  /**
  This contains all members of Verb that do not require information about the verb parameter types.

  See Verb for detailed information. */
  class VerbBase : detail::Noncopyable {
  public:
    /// Returns human-readable name for this Verb.
    const char *NameGet() const { return m_name; }

    /// Gets this verb's Dive version.
    /** Guaranteed to return NULL if this verb is a Dive or Bubble. */
    const VerbBase *DiveGet() const { return m_dive; }
    /// Gets this verb's Bubble version.
    /** Guaranteed to return NULL if this verb is a Dive or Bubble. */
    const VerbBase *BubbleGet() const { return m_bubble; }

  private:
    template <typename Parameters> friend class Verb;
    VerbBase(const char *name) : m_name(name), m_dive(0), m_bubble(0) {};
    VerbBase(const char *name, const VerbBase *dive, const VerbBase *bubble) : m_name(name), m_dive(dive), m_bubble(bubble) {};

    const char *m_name;
    const VerbBase *m_dive;
    const VerbBase *m_bubble;
  };

  /// Event handle for storing metainformation and providing callbacks.
  /**
  A Handle is provided for every event handler call. This handle has a limited lifetime - it is available only until the termination of that event call.*/
  class Handle : detail::Noncopyable {
  public:
    /// Returns the Layout this event refers to.
    /** If this is part of a Dive or Bubble series, this will return the Layout that is the destination of the series. */
    Layout *TargetGet() const { return m_target; }
    /// Returns the Verb this event refers to.
    /** If this is part of a Dive or Bubble series, this will return the specific Verb that is being called. */
    const VerbBase *VerbGet() const { return m_verb; }
    
  private:
    friend class Layout;

    Handle(Layout *target, const VerbBase *verb) : m_target(target), m_verb(verb) {}
    ~Handle() {} // TODO - clean up handles in Lua?

    Layout *m_target;
    const VerbBase *m_verb;
  };

  /// Type-specific Verb class.
  /** A Verb represents an event category that can be called on a Layout. */
  template <typename Parameters> class Verb : public VerbBase {
  private:
    friend class VerbPackage<Parameters>;
    Verb(const char *name, const Verb<Parameters> *dive, const Verb<Parameters> *bubble) : VerbBase(name, dive, bubble) { };

  public:
    /// Constructor for standalone Verbs.
    /** Do not call manually - at the moment, it is required that you use FRAMES_VERB_DECLARE/FRAMES_VERB_DEFINE. */
    Verb(const char *name) : VerbBase(name) { };
    
    /// Convenience typedef for the function type which is needed to attach to this Verb.
    typedef typename detail::FunctionPrefix<Handle*, Parameters>::T TypeHandler;
    /// Convenience typedef for the delegate type which is needed to attach to this Verb.
    typedef Delegate<typename detail::FunctionPrefix<Handle*, Parameters>::T> TypeDelegate;
  };

  template <typename Parameters> class VerbPackage : public Verb<Parameters> {
  public:
    /// Constructor for VerbPackages.
    /** Do not call manually - at the moment, it is required that you use FRAMES_VERB_DECLARE_BUBBLE/FRAMES_VERB_DEFINE_BUBBLE. */
    VerbPackage(const char *name, const char *nameDive, const char *nameBubble) : Verb<Parameters>(name, &Dive, &Bubble), Dive(nameDive), Bubble(nameBubble) { };

    /// Dive version of this VerbPackage.
    Verb<Parameters> Dive;
    /// Bubble version of this VerbPackage.
    Verb<Parameters> Bubble;
  };

  /// Should be called before any uses of FRAMES_VERB_DECLARE or FRAMES_VERB_DECLARE_BUBBLE.
  /**
  This is a convenience macro that will place layout verbs into a Verb class.
  It's not strictly necessary if you want a different Verb layout - Verbs are just singletons and don't care where they live - but if you want to follow library conventions, its use is recommended. */
  #define FRAMES_VERB_DECLARE_BEGIN \
    class Event { \
      Event();  /* intentionally left undefined, this is not meant to be instantiated */ \
      ~Event(); \
    public:

  /// Declares a verb with the given name and parameter list.
  /**
  Example usages:

  FRAMES_VERB_DECLARE(HelloWorld, ());
  FRAMES_VERB_DECLARE(HelloWorldWithParameter, (int parameterName));
  FRAMES_VERB_DECLARE(HelloWorldWithParameters, (int parameter1, const std::string &parameter2)); */
  #define FRAMES_VERB_DECLARE(eventname, paramlist) \
    static Verb<void paramlist> eventname;

  /// Declares a Dive/Bubble verb with the given name and parameter list.
  /** Besides macro name, usage is identical to that of FRAMES_VERB_DECLARE. */
  #define FRAMES_VERB_DECLARE_BUBBLE(eventname, paramlist) \
    static VerbPackage<void paramlist> eventname;

  /// Should be called after any uses of FRAMES_VERB_DECLARE or FRAMES_VERB_DECLARE_BUBBLE.
  /** See FRAMES_VERB_DECLARE_BEGIN for details. */
  #define FRAMES_VERB_DECLARE_END \
    };
}

#endif
