// Smart pointer

#ifndef FRAMES_PTR
#define FRAMES_PTR

#include "frames/noncopyable.h"

namespace Frames {
  /// Basic refcounting pointer.
  /** Requires that T conform to the interface of Refcountable<T>. The easiest way to accomplish this is to inherit T from Refcountable<T>. */
  template <typename T> class Ptr {
  public:
    typedef Ptr<T> this_type;

    /// Initializes to an empty pointer.
    Ptr() : p(0) { }
    /// Initializes to an instance of T and takes ownership of that object.
    /** If you have a T you don't want to be owned by Ptr, call ->RefAdd() on that T before removing all Ptr references.

    Obviously, it will now be your responsibility to call ->RefRelease() the appropriate number of times. */
    explicit Ptr(T *p) : p(p) { if (p) p->RefAdd(); }
    Ptr(const Ptr &rhs) : p(rhs.p) { if (p) p->RefAdd(); }
    template <typename U> Ptr(const Ptr<U> &rhs) : p(rhs.p) { if (p) p->RefAdd(); }
    ~Ptr() { if (p) p->RefRelease(); }

    Ptr &operator=(const Ptr &rhs) { this_type(rhs).Swap(*this); return *this; }

    /// Clears this pointer.
    void Reset() { this_type().Swap(*this); }
    /// Sets this pointer to a new object.
    void Reset(T *rhs) { this_type(rhs).Swap(*this); }

    /// Gets the contents of this pointer, or NULL if it points to nothing valid.
    T *Get() const { return p; }

    // todo: assert
    T &operator*() const { return *p; }
    T *operator->() const { return p; }

    void Swap(Ptr &rhs) { T *t = p; p = rhs.p; rhs.p = t; }

    typedef T *this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const { return !p ? 0 : &this_type::p; }
    bool operator!() const { return !p; }

  private:
    template <typename U> friend class Ptr;

    T *p;
  };

  /// Provides a pre-built intrusive refcount class for compatibility with Ptr.
  /** For efficiency, this class relies on the Curiously Recurring Template Pattern, and therefore must be specialized on the class that is inheriting from it.
  
  Example:

  class OurInternalClass : public Ptr<OurInternalClass> { }; */
  template <typename T> class Refcountable : detail::Noncopyable {
  public:
    Refcountable() : m_refs(0) { }
    ~Refcountable() { };

    /// Increments the internal refcount.
    /** Exposed so this can be linked with your own reference count system.

    Also useful for forcing an object to never be automatically cleaned up, in the case of global persistent singletons. */
    void RefAdd() { ++m_refs; }
    /// Decrements the internal refcount.
    /** As this may destroy this object, we strongly advise against accessing this object after calling this function. */
    void RefRelease() { --m_refs; if (!m_refs) delete (T*)this; }  // Casts to T* in order to avoid requiring a virtual destructor, or indeed, any virtual functions.

  private:
    int m_refs;
  };
}

#endif
