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

#include <deque>

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
  explicit pcbuf(size_t capacity) : cap_(capacity) {}

  ~pcbuf() {}

  size_t size() const {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock)
        return q_.size();
    }
    return 0; /* NOTREACHED */
  }

  size_t capacity() const {
    return cap_;
  }

  bool empty() const {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock)
        return q_.empty();
    }
    return false; /* NOTREACHED */
  }

  void clear() {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock)
        q_.clear();
    }
    cond_.notify_all();
  }

  void push(const T& value) {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        while (q_.size() >= cap_)
          cond_.wait(m_);
        q_.push_back(value);
      }
    }
    cond_.notify_all();
  }

  bool push(const T& value, double second) {
    double start = static_cast<double>(system::time::get_clock_time());
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        while (q_.size() >= cap_) {
          second -= static_cast<double>(system::time::get_clock_time()) - start;
          if (second <= 0 || !cond_.wait(m_, second))
            return false;
        }
        q_.push_back(value);
      }
    }
    cond_.notify_all();
    return true;
  }

  void pop(T& value) {
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        while (q_.empty())
          cond_.wait(m_);
        value = q_.front();
        q_.pop_front();
      }
    }
    cond_.notify_all();
  }

  bool pop(T& value, double second) {
    double start = static_cast<double>(system::time::get_clock_time());
    {
      pfi::concurrent::scoped_lock lock(m_);
      if (lock) {
        while (q_.empty()) {
          second -= static_cast<double>(system::time::get_clock_time()) - start;
          if (second <= 0 || !cond_.wait(m_, second))
            return false;
        }
        value = q_.front();
        q_.pop_front();
      }
    }
    cond_.notify_all();
    return true;
  }

private:
  const size_t cap_;
  std::deque<T> q_;
  mutable mutex m_;
  condition cond_;
};

} // concurrent
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_CONCURRENT_PCBUF_H_
