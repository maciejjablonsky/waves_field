#pragma once
#include <clock.hpp>

namespace wf::components
{
class movement
{
  public:
    using timepoint_type = decltype(std::declval<const wf::clock>().current());

  private:
    timepoint_type start_{};
    timepoint_type end_{};
    bool is_finite_ = true;

  public:
    void mark_as_infinite();
    void set_range(timepoint_type start, timepoint_type end);
    bool is_finite() const;
    bool is_infinite() const;
    bool is_happening(timepoint_type right_now) const;
    void stop();
};
} // namespace wf::components