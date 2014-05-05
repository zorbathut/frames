// The Layout class

#ifndef FRAMES_LAYOUT
#define FRAMES_LAYOUT

#include "frames/delegate.h"
#include "frames/detail.h"
#include "frames/event.h"
#include "frames/input.h"
#include "frames/noncopyable.h"
#include "frames/vector.h"

#include "boost/static_assert.hpp"

#include <vector>
#include <set>
#include <map>
#include <string>

namespace Frames {
  class Environment;
  struct Rect;
  class Layout;

  template <typename T> T *Cast(Layout *layout);
  template <typename T> const T *Cast(const Layout *layout);

  namespace detail {
    class Renderer;
    class Rtti;

    template <typename T> const Rtti *InitHelper();
  }

  #define FRAMES_DECLARE_RTTI() \
    static detail::Rtti s_rtti; \
    static const detail::Rtti *RttiStaticGet() { return &s_rtti; } \
    virtual const detail::Rtti *RttiVirtualGet() const { return &s_rtti; } \
    template <typename T> friend T *Cast(Layout *); \
    template <typename T> friend const T *Cast(const Layout *); \
    template <typename T> friend const detail::Rtti *detail::InitHelper(); \
  public: \
    /** \quick Returns a human-readable type string for this class. */ \
    static const char *TypeStaticGet(); \
    /** \quick Returns a human-readable type string for this instance. */ \
    virtual const char *TypeGet() const { return TypeStaticGet(); } \
  private:

  /// Base class containing location and layout information for all Frames elements.
  /**
  A Layout includes all information needed to calculate and update the position and ordering of an element in an Environment. The vast majority of all Layouts are actually \ref Frame "Frames"; the only exception is the Root layout.
  
  Layout includes all update-related functions as protected members, most of which are exposed by Frame.*/
  class Layout : detail::Noncopyable {
  public:

    FRAMES_VERB_DECLARE_BEGIN
      /// Signals when a frame's edges move.
      /** WARNING: Unlike most events, Move may not signal immediately after a state change. Move is guaranteed to fire before Environment::Prepare() returns. */
      FRAMES_VERB_DECLARE(Move, ());

      /// Signals when a frame's size changes.
      /** Since size cannot change without the frame's edges moving, a Size signal is always associated with a Move signal.
      
      WARNING: Unlike most events, Size may not signal immediately after a state change. Size is guaranteed to fire before Environment::Prepare() returns. */
      FRAMES_VERB_DECLARE(Size, ());

      /// Signals when the mouse enters a frame.
      /** If a mouse moves from one frame to another, MouseOver is guaranteed to fire after MouseOut. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseOver, ());

      /// Signals when the mouse moves while inside a frame.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMove, (const Vector &pt));

      /// Signals when the mouse moves while outside a frame, after pressing a button inside that frame and keeping it held.
      /** This functionality is intended for drag-and-drop applications. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseMoveoutside, (const Vector &pt));

      /// Signals when the mouse leaves a frame.
      /** If a mouse moves from one frame to another, MouseOver is guaranteed to fire after MouseOut. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseOut, ());

      /// Signals immediately after MouseButtonUp(0). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseLeftUp, ());
      /// Signals immediately after MouseButtonUpoutside(0). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseLeftUpoutside, ());
      /// Signals immediately after MouseButtonDown(0). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseLeftDown, ());
      /// Signals immediately after MouseButtonClick(0). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseLeftClick, ());

      /// Signals immediately after MouseButtonUp(1). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMiddleUp, ());
      /// Signals immediately after MouseButtonUpoutside(1). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMiddleUpoutside, ());
      /// Signals immediately after MouseButtonDown(1). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMiddleDown, ());
      /// Signals immediately after MouseButtonClick(1). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseMiddleClick, ());

      /// Signals immediately after MouseButtonUp(2). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseRightUp, ());
      /// Signals immediately after MouseButtonUpoutside(2). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseRightUpoutside, ());
      /// Signals immediately after MouseButtonDown(2). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseRightDown, ());
      /// Signals immediately after MouseButtonClick(2). See that verb's documentation for details.
      FRAMES_VERB_DECLARE_BUBBLE(MouseRightClick, ());

      /// Signals that a mouse button has been released while inside a frame.
      FRAMES_VERB_DECLARE_BUBBLE(MouseButtonUp, (int button));
      /// Signals that a mouse button has been released while outside a frame, after pressing that button inside that frame and then moving the mouse out of the frame.
      /** This functionality is intended for drag-and-drop applications. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseButtonUpoutside, (int button));
      /// Signals that a mouse button has been pressed while inside a frame.
      FRAMES_VERB_DECLARE_BUBBLE(MouseButtonDown, (int button));
      /// Signals that a mouse button has been clicked while inside a frame.
      /** This is currently defined as "press, then release, while inside the same frame", and is provided as a commonly-needed convenience verb. */
      FRAMES_VERB_DECLARE_BUBBLE(MouseButtonClick, (int button));

      /// Signals that the mouse wheel has been moved while inside a frame.
      FRAMES_VERB_DECLARE_BUBBLE(MouseWheel, (int delta));

      /// Signals that a key has been pressed while a frame has the key focus.
      /** This verb is recommended for non-text applications, as it reports the key ID and not the unicode text that was entered. */
      FRAMES_VERB_DECLARE_BUBBLE(KeyDown, (Input::Key kev));
      /// Signals that a keypress has repeated while a frame has the key focus.
      /** This verb is recommended for non-text applications, as it reports the key ID and not the unicode text that was entered. */
      FRAMES_VERB_DECLARE_BUBBLE(KeyRepeat, (Input::Key kev));
      /// Signals that a key has been released while a frame has the key focus.
      /** This verb is recommended for non-text applications, as it reports the key ID and not the unicode text that was entered. */
      FRAMES_VERB_DECLARE_BUBBLE(KeyUp, (Input::Key kev));

      /// Signals that text has been typed while a frame has the key focus.
      /** This verb is recommended for text applications, as it provides a Unicode string potentially containing multiple entered characters.*/
      FRAMES_VERB_DECLARE_BUBBLE(KeyText, (const std::string &text));
    FRAMES_VERB_DECLARE_END

  private:
    FRAMES_DECLARE_RTTI();
    friend class Environment; // access WidthDefaultSet/HeightDefaultSet, construction
    friend class Frame; // constructor, private functions that modify state
    friend class Mask; // solely for MouseMasking
    
    struct FrameOrderSorter { bool operator()(const Layout *lhs, const Layout *rhs) const; };
    
    // Event system
    
    // Must function properly when copied by value!
    struct Callback {
    public:
      // NOTE: this works because delegate is POD
      Callback() : m_priority(0), m_destroy(false), m_lock(0) { }
      ~Callback() { }
      
      template<typename T> static Callback CreateNative(Delegate<T> din, float priority) {
        BOOST_STATIC_ASSERT(sizeof(Delegate<T>) == sizeof(Delegate<void ()>));
        
        Callback rv;
        rv.m_priority = priority;

        rv.DelegateGet<T>() = din;
        
        return rv;
      }
      
      struct Sorter {
        bool operator()(const Callback &lhs, const Callback &rhs) const;
      };
      
      void Call(Handle *eh) const {
        DelegateGet<void(Handle *)>()(eh);
      }
      
      template <typename P1> void Call(Handle *eh, P1 p1) const {
        DelegateGet<void(Handle *, P1)>()(eh, p1);
      }
      
      template<typename T> bool NativeCallbackEqual(Delegate<T> din) const {
        return *reinterpret_cast<const Delegate<T> *>(&m_delegate) == din;
      }
      
      float PriorityGet() const { return m_priority; }
      
      bool DestroyFlagGet() const { return m_destroy; }
      void DestroyFlagSet() const { m_destroy = true; }
      
      bool LockFlagGet() const { return m_lock != 0; }
      void LockFlagIncrement() const { ++m_lock; }  // must store result and pass it to LockFlagDecrement
      void LockFlagDecrement() const { --m_lock; }
      
      void Teardown(Environment *env) const;  // cleans up the underlying resources, if any. Not the same as a destructor! This isn't RAII for efficiency reasons. Environment provided for debug hooks.
      
    private:
      // the delegate itself - this is not the right type, it is casted appropriately by DelegateGet
      Delegate<void()> m_delegate;
      template<typename T> Delegate<T> &DelegateGet() {
        return reinterpret_cast<Delegate<T> &>(m_delegate);
      }
      template<typename T> const Delegate<T> &DelegateGet() const {
        return reinterpret_cast<const Delegate<T> &>(m_delegate);
      }

      float m_priority;
      
      // "mutable" because they don't contribute to the sorting, so we need them to be modifiable even when used as multiset key
      mutable bool m_destroy;
      mutable int m_lock;
    };
    
    typedef std::multiset<Callback, Callback::Sorter> EventMultiset;
    typedef std::map<const VerbBase *, std::multiset<Callback, Callback::Sorter> > EventLookup;
    
    class FEIterator {
    public:
      FEIterator();
      FEIterator(Layout *target, const VerbBase *event);
      FEIterator(const FEIterator &itr);
      void operator=(const FEIterator &itr);
      ~FEIterator();
      
      const Callback &Get() const;
      bool Complete() const;
      void Next();
      
    private:
      void IteratorUnlock(EventMultiset::iterator itr); // iterator may be invalidated by this function
      void IndexNext();
      
      Layout *LayoutGet();
      const VerbBase *EventGet();
      
      enum State { STATE_DIVE, STATE_MAIN, STATE_BUBBLE, STATE_COMPLETE };
      State m_state;
      
      std::vector<Layout *> m_dives;
      int m_diveIndex;
      
      Layout *m_target;
      const VerbBase *m_event;
      
      EventMultiset::iterator m_current;
      EventMultiset::iterator m_last;
    };
    
  public:
    // --------- Identification

    /// Returns the name of this layout.
    const std::string &NameGet() const { return m_name; }

    // --------- Layout accessors

    /// Returns the position of an anchor along a given axis.
    /** See \ref layoutbasics for details. */
    float PointGet(Axis axis, float pt) const;
    /// Returns the left edge's position.
    float LeftGet() const { return PointGet(X, 0); }
    /// Returns the right edge's position.
    float RightGet() const { return PointGet(X, 1); }
    /// Returns the top edge's position.
    float TopGet() const { return PointGet(Y, 0); }
    /// Returns the bottom edge's position.
    float BottomGet() const { return PointGet(Y, 1); }
    /// Returns the bounds of the layout.
    Rect BoundsGet() const;

    /// Returns the size along a given axis.
    float SizeGet(Axis axis) const;
    /// Returns the width.
    float WidthGet() const { return SizeGet(X); }
    /// Returns the height.
    float HeightGet() const { return SizeGet(Y); }

    /// Returns the parent. Guaranteed to be non-null unless this is Root.
    Layout *ParentGet() const { return m_parent; }

    /// Returns the highest input-accepting frame located at the given coordinates.
    Layout *LayoutUnderGet(float x, float y);

    // RetrieveHeight/RetrieveWidth/RetrievePoint/etc?

    /// Type used to store a list of children, sorted from bottom to top. Conforms to std::set's interface; may not be a std::set.
    typedef std::set<Layout *, FrameOrderSorter> ChildrenList;

    /// Returns the children of this frame.
    /** Pass in "true" for implementation to get only children with the Implementation flag set. Otherwise, you'll get only children without that flag set. */
    const ChildrenList &ChildrenGet(bool implementation = false) { return implementation ? m_children_implementation : m_children_nonimplementation; }
    
    // --------- State

    /// Sets the visibility flag.
    /** Invisible frames will not render or accept input, nor will their children render nor accept input. */
    void VisibleSet(bool visible);
    /// Gets the visibility flag.
    bool VisibleGet() const { return m_visible; }

    /// Sets the alpha multiplier.
    /** Changing the alpha multiplier causes this frame, and all its children, to render partially transparent. Note that this is done on a per-frame basis and will not result in layer-perfect results. */
    void AlphaSet(float alpha) { m_alpha = alpha; }
    /// Gets the alpha multiplier.
    float AlphaGet() const { return m_alpha; }

    // --------- Events

    /// Attaches a handler to a verb on this layout.
    template <typename Parameters> void EventAttach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority = 0.0);
    /// Detaches a handler from a verb on this layout.
    template <typename Parameters> void EventDetach(const Verb<Parameters> &event, typename Verb<Parameters>::TypeDelegate handler, float priority = detail::Undefined);
    
    /// Triggers all attached handlers for a given verb.
    inline void EventTrigger(const Verb<void ()> &event);
    /// Triggers all attached handlers for a given verb.
    template <typename P1> void EventTrigger(const Verb<void (P1)> &event, typename detail::MakeConstRef<P1>::T p1);

    /// Determines if a verb is hooked.
    bool EventHookedIs(const VerbBase &event) const;

    // --------- Misc

    /// Mode for input handling.
    /**
    IM_NONE indicates that the frame should not accept input of any kind; any input will pass through transparently.
    IM_ALL indicates that the frame should accept all kinds of input, and input will not pass to a frame below this one.*/
    enum InputMode { IM_NONE, IM_ALL, IM_COUNT };
    /// Sets the input mode.
    void InputModeSet(InputMode imode);
    /// Gets the input mode.
    inline InputMode InputModeSet() const { return m_inputMode; }

    /// Returns the environment.
    Environment *EnvironmentGet() const { return m_env; }

    // --------- Debug

    /// Dumps comprehensive layout information to the debug log.
    void DebugDumpLayout() const;

    /// Constructs and return the full named path of this layout.
    std::string DebugNameGet() const;

  protected:
    /// Set default size of an axis.
    /** Default sizing is used for frames that semantically have a "normal" size, but may be resized arbitrary by the user. The frame will fall back to the default size if its size is left undefined otherwise (either via explicit sizing or via sufficient linking to fix the size.)

    This should generally not be exposed to the end-user of a class - it is intended as an internal implementation tool. */
    void SizeDefaultSet(Axis axis, float size);
    /// Set default width. See SizeDefaultSet for details.
    void WidthDefaultSet(float size) { return SizeDefaultSet(X, size); }
    /// Set default height. See SizeDefaultSet for details.
    void HeightDefaultSet(float size) { return SizeDefaultSet(Y, size); }

    /// Called when this frame is rendered.
    /** Overload this to create your own frame types. Must call (super)::RenderElement before it does its own work. */
    virtual void RenderElement(detail::Renderer *renderer) const {};
    /// Called before this frame's children have been rendered.
    /** Overload this for pre-render setup. If this frame has no children, may not be called at all. Must call (super)::RenderElementPreChild before it does its own work. */
    virtual void RenderElementPreChild(detail::Renderer *renderer) const {};
    /// Called after this frame's children have been rendered.
    /** Overload this for post-render cleanup. If this frame has no children, may not be called at all. Must call (super)::RenderElementPreChild *after* it does its own work. */
    virtual void RenderElementPostChild(detail::Renderer *renderer) const {};

  private:
    Layout(const std::string &name, Environment *env);
    virtual ~Layout();

    // These need to be defined here because Layout actually contains the implementation, but the documentation and function definitions need to show up in frame.h for the sake of documentation.
    // This is a bit ugly, but thankfully they can all be inlined.
    // zinternal prefix so they show up at the bottom of sorting, in either case-sensitive or case-insensitive mode.
    void zinternalPinSet(Axis axis, float mypt, const Layout *link, float theirpt, float offset = 0.f);
    void zinternalPinClear(Axis axis, float mypt);
    void zinternalPinClear(Anchor anchor);
    void zinternalPinClearAll(Axis axis);

    void zinternalPinSet(Anchor myanchor, const Layout *link, Anchor theiranchor);
    void zinternalPinSet(Anchor myanchor, const Layout *link, Anchor theiranchor, float xofs, float yofs);
    void zinternalPinSet(Anchor myanchor, const Layout *link, float theirx, float theiry);
    void zinternalPinSet(Anchor myanchor, const Layout *link, float theirx, float theiry, float xofs, float yofs);
    void zinternalPinSet(float myx, float myy, const Layout *link, Anchor theiranchor);
    void zinternalPinSet(float myx, float myy, const Layout *link, Anchor theiranchor, float xofs, float yofs);
    void zinternalPinSet(float myx, float myy, const Layout *link, float theirx, float theiry);
    void zinternalPinSet(float myx, float myy, const Layout *link, float theirx, float theiry, float xofs, float yofs);

    void zinternalSizeSet(Axis axis, float size);
    void zinternalWidthSet(float size) { return zinternalSizeSet(X, size); }
    void zinternalHeightSet(float size) { return zinternalSizeSet(Y, size); }
    void zinternalSizeClear(Axis axis);

    void zinternalConstraintClearAll();

    void zinternalParentSet(Layout *layout);

    void zinternalNameSet(const std::string &name) { m_name = name; }

    void zinternalLayerSet(float layer);
    float zinternalLayerGet() const { return m_layer; }

    void zinternalImplementationSet(bool implementation);
    bool zinternalImplementationGet() const { return m_implementation; }

    void zinternalObliterate();

    // Layout utility
    void ChildAdd(Layout *child);
    void ChildRemove(Layout *child);

    // Rendering
    void Render(detail::Renderer *renderer) const;

    // Mask-related
    void MouseMaskingFullSet(bool mask) { m_fullMouseMasking = mask; }
    bool MouseMaskingFullGet() { return m_fullMouseMasking; }
    virtual bool MouseMaskingTest(float x, float y) { return true; }

    // Layout engine - note that this is used heavily by Frame!
    void Invalidate(Axis axis);
    void ObliterateDetach(); // Detach this layout from all layouts
    void ObliterateExtract();  // Detach everything that refers to this layout
    void ObliterateExtractAxis(Axis axis);  // Detach everything that refers to this axis
    void ObliterateExtractFrom(Axis axis, const Layout *layout);
    void Resolve();
    struct AxisData {
      AxisData() : size_cached(detail::Undefined), size_set(detail::Undefined), size_default(40) { };

      mutable float size_cached;

      struct Connector {
        Connector() : link(0), point_mine(detail::Undefined), point_link(detail::Undefined), offset(detail::Undefined), cached(detail::Undefined) { };

        const Layout *link;
        float point_mine;
        float point_link;
        float offset;

        mutable float cached;
      };
      Connector connections[2];

      float size_set;
      float size_default;

      typedef std::multiset<Layout *> ChildrenList;
      mutable ChildrenList children;
    };
    AxisData m_axes[2];
    mutable bool m_resolved;  // whether *this* frame has its layout completely determined

    // Layout events
    mutable float m_last_width, m_last_height;
    mutable float m_last_x, m_last_y;

    // Layer/parenting engine
    float m_layer;
    bool m_implementation;
    unsigned int m_constructionOrder; // This is used to create consistent results when frames are Z-conflicting
    Layout *m_parent;
    bool m_visible;
    ChildrenList m_children;  // Authoritative
    ChildrenList m_children_implementation; // Provided only for ChildrenGet
    ChildrenList m_children_nonimplementation; // Provided only for ChildrenGet

    // Rendering effects
    float m_alpha;

    // Input
    bool m_fullMouseMasking;
    InputMode m_inputMode;

    // Naming system
    std::string m_name;
    
    // Event system
    EventLookup m_events;
    void EventDestroy(EventLookup::iterator eventTable, EventMultiset::iterator toBeRemoved);

    // Obliterate buffering (sort of related to the event system) - todo make this take up less space
    int m_obliterate_lock;
    bool m_obliterate_buffered;
    void Obliterate_Lock();
    void Obliterate_Unlock(); // note that, after calling this function, the frame might eat itself

    // Global environment
    Environment *m_env;
  };

  // Debug code
  #ifdef _MSC_VER
    #define FRAMES_LAYOUT_ASSERT(x, errstring, ...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (EnvironmentGet()->LogError(detail::Format(errstring, __VA_ARGS__))))
    #define FRAMES_LAYOUT_CHECK(x, errstring, ...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (EnvironmentGet()->LogError(detail::Format(errstring, __VA_ARGS__))))

    #define FRAMES_ERROR(...) EnvironmentGet()->LogError(detail::Format(__VA_ARGS__))
    #define FRAMES_DEBUG(...) EnvironmentGet()->LogDebug(detail::Format(__VA_ARGS__))
  #else
    #define FRAMES_LAYOUT_ASSERT(x, errstring, args...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (EnvironmentGet()->LogError(detail::Format(errstring, ## args))))
    #define FRAMES_LAYOUT_CHECK(x, errstring, args...) (FRAMES_EXPECT(!!(x), 1) ? (void)(1) : (EnvironmentGet()->LogError(detail::Format(errstring, ## args))))

    #define FRAMES_ERROR(args...) EnvironmentGet()->LogError(detail::Format(args))
    #define FRAMES_DEBUG(args...) EnvironmentGet()->LogDebug(detail::Format(args))
  #endif
}

#include "frames/layout_template_inline.h"

#endif
