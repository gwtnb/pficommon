// Copyright (c)2008-2011, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef INCLUDE_GUARD_PFI_LANG_REF_H_
#define INCLUDE_GUARD_PFI_LANG_REF_H_

#include <functional>
#ifdef __GLIBCXX__ // libstdc++
#include <tr1/functional>
#elif defined(_LIBCPP_VERSION) // libc++
#include <type_traits>
#endif

namespace pfi {
namespace lang {

#ifdef __GLIBCXX__ // libstdc++

template <class T>
class reference_wrapper : public ::std::tr1::reference_wrapper<T> {
  typedef ::std::tr1::reference_wrapper<T> base;

public:
  explicit reference_wrapper(T& x) : base(x) {}
  T* get_pointer() const { return &this->get(); }
};

#else

template <class T>
class reference_wrapper {
public:
  reference_wrapper(T& x) : p_(&x) {}

  operator T&() const { return *p_; }
  T& get() const { return *p_; }
  T* get_pointer() const { return p_; }

  template <class U = T>
  typename std::result_of<U& ()>::type
  operator()() const {
    return (*p_)();
  }

  template <class A>
  typename std::result_of<T& (A&)>::type
  operator()(A& a) const {
    return call_op(a);
  }
  template <class A>
  typename std::result_of<T& (const A&)>::type
  operator()(const A& a) const {
    return call_op(a);
  }

  template <class A0, class A1>
  typename std::result_of<T& (A0&, A1&)>::type
  operator()(A0& a0, A1& a1) const {
    return call_op(a0, a1);
  }
  template <class A0, class A1>
  typename std::result_of<T& (A0&, const A1&)>::type
  operator()(A0& a0, const A1& a1) const {
    return call_op(a0, a1);
  }
  template <class A0, class A1>
  typename std::result_of<T& (const A0&, A1&)>::type
  operator()(const A0& a0, A1& a1) const {
    return call_op(a0, a1);
  }
  template <class A0, class A1>
  typename std::result_of<T& (const A0&, const A1&)>::type
  operator()(const A0& a0, const A1& a1) const {
    return call_op(a0, a1);
  }

  template <class A0, class A1, class A2>
  typename std::result_of<T& (A0&, A1&, A2&)>::type
  operator()(A0& a0, A1& a1, A2& a2) const {
    return call_op(a0, a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (A0&, A1&, const A2&)>::type
  operator()(A0& a0, A1& a1, const A2& a2) const {
    return call_op(a0, a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (A0&, const A1&, A2&)>::type
  operator()(A0& a0, const A1& a1, A2& a2) const {
    return call_op(a0, a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (A0&, const A1&, const A2&)>::type
  operator()(A0& a0, const A1& a1, const A2& a2) const {
    return call_op(a0, a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (const A0&, A1&, A2&)>::type
  operator()(const A0& a0, A1& a1, A2& a2) const {
    return call_op(a0, a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (const A0&, A1&, const A2&)>::type
  operator()(const A0& a0, A1& a1, const A2& a2) const {
    return call_op(a0, a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (const A0&, const A1&, A2&)>::type
  operator()(const A0& a0, const A1& a1, A2& a2) const {
    return call_op(a0, a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (const A0&, const A1&, const A2&)>::type
  operator()(const A0& a0, const A1& a1, const A2& a2) const {
    return call_op(a0, a1, a2);
  }

private:
  template <class A>
  typename std::result_of<T& (A&)>::type
  call_op(A& a,
          typename std::enable_if<std::is_member_function_pointer<T>::value, A>::type* = 0) const
  {
    return (a.**p_)();
  }
  template <class A>
  typename std::result_of<T& (A&)>::type
  call_op(A* a,
          typename std::enable_if<std::is_member_function_pointer<T>::value, A>::type* = 0) const
  {
    return (a->**p_)();
  }
  template <class A>
  typename std::result_of<T& (A&)>::type
  call_op(A& a,
          typename std::enable_if<!std::is_member_function_pointer<T>::value, A>::type* = 0) const
  {
    return (*p_)(a);
  }

  template <class A0, class A1>
  typename std::result_of<T& (A0&, A1&)>::type
  call_op(A0& a0, A1& a1,
          typename std::enable_if<std::is_member_function_pointer<T>::value, A0>::type* = 0) const
  {
    return (a0.**p_)(a1);
  }
  template <class A0, class A1>
  typename std::result_of<T& (A0&, A1&)>::type
  call_op(A0* a0, A1& a1,
          typename std::enable_if<std::is_member_function_pointer<T>::value, A0>::type* = 0) const
  {
    return (a0->**p_)(a1);
  }
  template <class A0, class A1>
  typename std::result_of<T& (A0&, A1&)>::type
  call_op(A0& a0, A1& a1,
          typename std::enable_if<!std::is_member_function_pointer<T>::value, A0>::type* = 0) const
  {
    return (*p_)(a0, a1);
  }

  template <class A0, class A1, class A2>
  typename std::result_of<T& (A0&, A1&, A2&)>::type
  call_op(A0& a0, A1& a1, A2& a2,
          typename std::enable_if<std::is_member_function_pointer<T>::value, A0>::type* = 0) const
  {
    return (a0.**p_)(a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (A0&, A1&, A2&)>::type
  call_op(A0* a0, A1& a1, A2& a2,
          typename std::enable_if<std::is_member_function_pointer<T>::value, A0>::type* = 0) const
  {
    return (a0->**p_)(a1, a2);
  }
  template <class A0, class A1, class A2>
  typename std::result_of<T& (A0&, A1&, A2&)>::type
  call_op(A0& a0, A1& a1, A2& a2,
          typename std::enable_if<!std::is_member_function_pointer<T>::value, A0>::type* = 0) const
  {
    return (*p_)(a0, a1, a2);
  }

  T* p_;
};
#endif

template <class T>
inline reference_wrapper<T> ref(T& r)
{
  return reference_wrapper<T>(r);
}

template <class T>
inline reference_wrapper<T> ref(reference_wrapper<T> r)
{
  return r;
}

template <class T>
inline reference_wrapper<const T> cref(const T& r)
{
  return reference_wrapper<const T>(r);
}

template <class T>
inline reference_wrapper<const T> cref(reference_wrapper<T> r)
{
  return reference_wrapper<const T>(r);
}

template <class T>
T* get_pointer(const reference_wrapper<T>& r)
{
  return r.get_pointer();
}

} // lang
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_LANG_REF_H_
