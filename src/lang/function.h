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

#ifndef INCLUDE_GUARD_PFI_LANG_FUNCTION_H_
#define INCLUDE_GUARD_PFI_LANG_FUNCTION_H_

#include <cassert>
#include <cstddef>
#include <functional>
#ifdef __GLIBCXX__ // libstdc++
#include <tr1/functional>
#endif

namespace pfi {
namespace lang {

namespace detail {
#ifdef _LIBCPP_VERSION // libc++
namespace function_ns = ::std;
#else
namespace function_ns = ::std::tr1;
#endif
}

template <class F>
class function : public detail::function_ns::function<F> {
  typedef detail::function_ns::function<F> base;

public:
  function() {}
  template <class Fn>
  function (const Fn& f) : base(f) {}

  template <class Fn>
  function& operator=(const Fn& f) {
    base::operator=(f);
    return *this;
  }

#ifdef _LIBCPP_VERSION
  friend bool operator==(const function& f, std::nullptr_t p) {
    return static_cast<const base&>(f) == p;
  }
  friend bool operator==(std::nullptr_t p, const function& f) {
    return f == p;
  }
  friend bool operator!=(const function& f, std::nullptr_t p) {
    return !(f == p);
  }
  friend bool operator!=(std::nullptr_t p, const function& f) {
    return f != p;
  }

  friend bool operator==(const function& f, int p) {
    assert(!p);
    return f == nullptr;
  }
  friend bool operator==(int p, const function& f) {
    assert(!p);
    return nullptr == f;
  }
  friend bool operator!=(const function& f, int p) {
    assert(!p);
    return f != nullptr;
  }
  friend bool operator!=(int p, const function& f) {
    assert(!p);
    return nullptr != f;
  }

  friend bool operator==(const function& f, long p) {
    assert(!p);
    return f == nullptr;
  }
  friend bool operator==(long p, const function& f) {
    assert(!p);
    return nullptr == f;
  }
  friend bool operator!=(const function& f, long p) {
    assert(!p);
    return f != nullptr;
  }
  friend bool operator!=(long p, const function& f) {
    assert(!p);
    return nullptr != f;
  }
#endif
};

} // lang
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_LANG_FUNCTION_H_
