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

#ifndef INCLUDE_GUARD_PFI_CONCURRENT_PCBUF_H_
#define INCLUDE_GUARD_PFI_CONCURRENT_PCBUF_H_

#include <vector>

#include "mutex.h"
#include "condition.h"
#include "lock.h"
#include "../lang/util.h"
#include "../system/time_util.h"

namespace pfi {
namespace concurrent {

template <class T>
class pcbuf : pfi::lang::noncopyable {
public:
  explicit pcbuf(size_t capacity) : q_(capacity), first_(0), last_(0), full_(capacity == 0) {}

  size_t size() const {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock)
        return size_();
    }
    return 0; /* NOTREACHED */
  }

  size_t capacity() const {
      return q_.size();
  }

  bool empty() const {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock)
        return empty_();
    }
    return false; /* NOTREACHED */
  }

  void clear() {
    bool should_notify = false;
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        if (full_)
          should_notify = true;
        first_ = last_ = 0;
        full_ = false;
      }
    }
    if (should_notify)
      cond_push_.notify_all();
  }

  void push(const T& value) {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        while (full_)
          cond_push_.wait(m_);
        push_(value);
      }
    }
    cond_pop_.notify();
  }

  bool push(const T& value, double second) {
    double start = static_cast<double>(system::time::get_clock_time());
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        while (full_) {
          second -= static_cast<double>(system::time::get_clock_time()) - start;
          if (second <= 0 || !cond_push_.wait(m_, second))
            return false;
        }
        push_(value);
      }
    }
    cond_pop_.notify_all();
    return true;
  }

  void pop(T& value) {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        while (empty_())
          cond_pop_.wait(m_);
        pop_(value);
      }
    }
    cond_push_.notify();
  }

  bool pop(T& value, double second) {
    double start = static_cast<double>(system::time::get_clock_time());
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        while (empty_()) {
          second -= static_cast<double>(system::time::get_clock_time()) - start;
          if (second <= 0 || !cond_pop_.wait(m_, second))
            return false;
        }
        pop_(value);
      }
    }
    cond_push_.notify();
    return true;
  }

private:
  std::size_t size_() const {
    std::ptrdiff_t cap = capacity();
    if (full_)
      return cap;
    return (last_ - first_ + cap) % cap;
  }

  bool empty_() const {
    return !full_ && first_ == last_;
  }

  void push_(const T& value) {
    q_[last_] = value;
    last_ = (last_ + 1) % capacity();
    full_ = first_ == last_;
  }

  void pop_(T& value) {
    value = q_[first_];
    first_ = (first_ + 1) % capacity();
    full_ = false;
  }

  std::vector<T> q_;
  std::ptrdiff_t first_, last_;
  bool full_;
  mutable mutex m_;
  condition cond_push_, cond_pop_;
};

} // concurrent
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_CONCURRENT_PCBUF_H_
