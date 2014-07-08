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

#ifndef FRAMES_EVENT
#define FRAMES_EVENT

#include "frames/detail.h"
#include "frames/noncopyable.h"
#include "frames/delegate.h"

namespace Frames {
  class Layout;
  
  template <typename Parameters> class VerbPackage;

  /// Represents a type of event.
  /** This contains all members of Verb that do not require information about the verb parameter types.

  See Verb for detailed information. */
  class VerbGeneric : detail::Noncopyable {
  public:
    /// Returns human-readable name for this Verb.
    const char *NameGet() const { return m_name; }

    /// Gets this verb's Dive variant.
    /** Returns null if this verb is a Dive or Bubble. */
    const VerbGeneric *DiveGet() const { return m_dive; }
    /// Gets this verb's Bubble variant.
    /** Returns null if this verb is a Dive or Bubble. */
    const VerbGeneric *BubbleGet() const { return m_bubble; }

  private:
    template <typename Parameters> friend class Verb;
    VerbGeneric(const char *name) : m_name(name), m_dive(0), m_bubble(0) {};
    VerbGeneric(const char *name, const VerbGeneric *dive, const VerbGeneric *bubble) : m_name(name), m_dive(dive), m_bubble(bubble) {};

    const char *m_name;
    const VerbGeneric *m_dive;
    const VerbGeneric *m_bubble;
  };

  /// Passed along with every event call for storing event metainformation and providing callbacks.
  /** This handle has a strictly limited lifetime - it is available only until the called function returns.*/
  class Handle : detail::Noncopyable {
  public:
    /// Returns the Layout this event refers to.
    /** If this is part of a Dive or Bubble series, this will return the Layout that is the destination of the series. */
    Layout *TargetGet() const { return m_target; }

    /// Returns the Verb this event refers to.
    /** If this is part of a Dive or Bubble series, this will return the specific Verb that is being called. */
    const VerbGeneric *VerbGet() const { return m_verb; }

    /// Returns the Layout this event is currently traversing.
    /** If this is not part of a Dive or Bubble series, this will be equivalent to TargetGet. */
    Layout *TargetContextGet() const { return m_targetContext; }

    /// Returns the Verb this event is currently traversing.
    /** If this is not part of a Dive or Bubble series, this will be equivalent to VerbGet. */
    const VerbGeneric *VerbContextGet() const { return m_verbContext; }
    
  private:
    friend class Layout;

    Handle(Layout *target, const VerbGeneric *verb) : m_target(target), m_verb(verb), m_targetContext(target), m_verbContext(verb) {}
    ~Handle() {} // TODO - clean up handles in Lua?

    void SetContext(Layout *target, const VerbGeneric *verb) {
      m_targetContext = target;
      m_verbContext = verb;
    }

    Layout *m_target;
    const VerbGeneric *m_verb;

    Layout *m_targetContext;
    const VerbGeneric *m_verbContext;
  };

  /// Represents a type of event with specific parameter typing.
  /** A Verb represents an event category that can be called on a Layout. */
  template <typename Parameters> class Verb : public VerbGeneric {
  private:
    friend class VerbPackage<Parameters>;
    Verb(const char *name, const Verb<Parameters> *dive, const Verb<Parameters> *bubble) : VerbGeneric(name, dive, bubble) { };

  public:
    /// Constructor for standalone Verbs.
    /** Do not call manually - at the moment, it is required that you use FRAMES_VERB_DECLARE/FRAMES_VERB_DEFINE. */
    Verb(const char *name) : VerbGeneric(name) { };
    
    /// Convenience typedef for the function type which is needed to attach to this Verb.
    typedef typename detail::FunctionPrefix<Handle*, Parameters>::T TypeHandler;
    /// Convenience typedef for the delegate type which is needed to attach to this Verb.
    typedef Delegate<typename detail::FunctionPrefix<Handle*, Parameters>::T> TypeDelegate;
  };

  /// Represents a type of event with specific parameter typing and Dive/Bubble behavior.
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
  /** This is a convenience macro that will place layout verbs into a Verb class.
  It's not strictly necessary if you want a different Verb layout - Verbs are just singletons and don't care where they live - but if you want to follow library conventions, its use is recommended. */
  #define FRAMES_VERB_DECLARE_BEGIN \
    /** \brief Container for class-specific verbs. */ \
    class Event { \
      Event();  /* intentionally left undefined, this is not meant to be instantiated */ \
      ~Event(); \
    public:

  /// Declares a verb with the given name and parameter list.
  /** Example usages:

  FRAMES_VERB_DECLARE(HelloWorld, ());
  FRAMES_VERB_DECLARE(HelloWorldWithParameter, (int parameterName));
  FRAMES_VERB_DECLARE(HelloWorldWithParameters, (int parameter1, const std::string &parameter2));
  
  After declaration, use FRAMES_VERB_DEFINE in a single .cpp file in order to instantiate the objects. */
  #define FRAMES_VERB_DECLARE(eventname, paramlist) \
    static Verb<void paramlist> eventname;

  /// Declares a Dive/Bubble verb with the given name and parameter list.
  /** Besides macro names, usage is identical to that of FRAMES_VERB_DECLARE.
  
  After declaration, use FRAMES_VERB_DEFINE_BUBBLE in a single .cpp file in order to instantiate the objects. */
  #define FRAMES_VERB_DECLARE_BUBBLE(eventname, paramlist) \
    static VerbPackage<void paramlist> eventname;

  /// Should be called after any uses of FRAMES_VERB_DECLARE or FRAMES_VERB_DECLARE_BUBBLE.
  /** See FRAMES_VERB_DECLARE_BEGIN for details. */
  #define FRAMES_VERB_DECLARE_END \
    };
}

#endif
