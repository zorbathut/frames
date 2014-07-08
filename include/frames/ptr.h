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

#ifndef FRAMES_PTR
#define FRAMES_PTR

#include "frames/noncopyable.h"

namespace Frames {
  /// Basic refcounted pointer.
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
    /// Standard copy constructor.
    /** Will add a reference. */
    Ptr(const Ptr &rhs) : p(rhs.p) { if (p) p->RefAdd(); }
    /// Type-converting copy constructor.
    /** Will add a reference. If the destination type cannot be implicitly converted from this type, will fail to compile. */
    template <typename U> Ptr(const Ptr<U> &rhs) : p(rhs.p) { if (p) p->RefAdd(); }
    /// Standard destructor.
    /** Will remove a reference, possibly destroying the object. */
    ~Ptr() { if (p) p->RefRelease(); }

    /// Standard assignment operator.
    /** Will both add and remove a reference, possibly destroying the assignment target. */
    Ptr &operator=(const Ptr &rhs) { this_type(rhs).Swap(*this); return *this; }

    /// Clears this pointer.
    void Reset() { this_type().Swap(*this); }
    /// Sets this pointer to a new object.
    void Reset(T *rhs) { this_type(rhs).Swap(*this); }

    /// Gets the contents of this pointer, or null if it points to nothing valid.
    T *Get() const { return p; }

    /// Dereferences the pointer.
    /** Undefined results if the pointer is currently null. */
    T &operator*() const { return *p; }
    /// Dereferences the pointer.
    /** Undefined results if the pointer is currently null. */
    T *operator->() const { return p; }

    /// Swaps two smart pointers.
    /** Guaranteed to not deallocate anything. */
    void Swap(Ptr &rhs) { T *t = p; p = rhs.p; rhs.p = t; }

    typedef T *this_type::*unspecified_bool_type;

    /// Implicit cast so this pointer can be used in a conditional.
    operator unspecified_bool_type() const { return !p ? 0 : &this_type::p; }
    /// Returns true if this pointer is null.
    bool operator!() const { return !p; }

  private:
    template <typename U> friend class Ptr;

    T *p;
  };

  /// Provides a pre-built intrusive refcounting interface for compatibility with Ptr.
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
