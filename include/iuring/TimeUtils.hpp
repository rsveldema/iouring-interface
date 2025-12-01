#pragma once

#include <chrono>
#include <functional>
#include <cassert>

namespace time_utils
{

static inline
std::chrono::nanoseconds get_current_time()
{
#if 1
  struct timespec now;
  clock_gettime( CLOCK_MONOTONIC_RAW, &now );
  return std::chrono::nanoseconds(((uint64_t)now.tv_sec * UINT64_C(1000000000)) + (uint64_t)now.tv_nsec);
#else
    std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const auto nanos =
        std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return nanos;
#endif
}

static inline
std::chrono::microseconds get_current_time_micros() {
    const auto now = get_current_time();
    return std::chrono::duration_cast<std::chrono::microseconds>(now);
}


static inline
std::chrono::milliseconds get_current_time_millis() {
    const auto now = get_current_time();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now);
}

class Timeout
{
public:
    Timeout(const std::chrono::microseconds& t)
    {
        m_deadline = get_current_time() + t;
    }

    bool elapsed() const
    {
        return get_current_time() >= m_deadline;
    }

    std::chrono::nanoseconds time_left() const
    {
        return m_deadline - get_current_time();
    }

private:
    std::chrono::nanoseconds m_deadline;
};


} // namespace ptp