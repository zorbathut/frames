// Smart pointer

#ifndef FRAME_PTR
#define FRAME_PTR

#include "frames/noncopyable.h"

namespace Frames {
  template <typename T> class Ptr {
  public:
    typedef Ptr<T> this_type;

    Ptr() : p(0) { }
    Ptr(T *p) : p(p) { if (p) p->Ref_Add(); }
    Ptr(const Ptr &rhs) : p(rhs.p) { if (p) p->Ref_Add(); }
    ~Ptr() { if (p) p->Ref_Release(); }

    Ptr &operator=(const Ptr &rhs) { this_type(rhs).swap(*this); return *this; }

    void reset() { this_type().swap(*this); }
    void reset(T *rhs) { this_type(rhs).swap(*this); }

    T *get() const { return p; }

    T &operator*() const { return *p; }
    T *operator->() const { return p; }

    void swap(Ptr &rhs) { T *t = p; p = rhs.p; rhs.p = t; }

    typedef T *this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const { return !p ? 0 : &this_type::p; }
    bool operator!() const { return !p; }

  private:

    T *p;
  };

  template <typename T> class Refcountable : Noncopyable {
  public:
    Refcountable() : m_refs(0) { }
    ~Refcountable() { };

  private:
    friend class Ptr<T>;
    int m_refs;
    void Ref_Add() { ++m_refs; }
    void Ref_Release() { --m_refs; if (!m_refs) delete (T*)this; }
  };
};

#endif
