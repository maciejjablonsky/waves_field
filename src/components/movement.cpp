#include "movement.hpp"

namespace wf
{
void components::movement::mark_as_infinite()
{
    is_finite_ = false;
}
void components::movement::set_range(timepoint_type start, timepoint_type end)
{
    start_ = start;
    end_   = end;
}
bool components::movement::is_finite() const
{
    return is_finite_;
}
bool components::movement::is_infinite() const
{
    return not is_finite_;
}
bool components::movement::is_happening(timepoint_type right_now) const
{
    return is_infinite() or ((start_ <= right_now) and (right_now < end_));
}
void components::movement::stop()
{
    is_finite_ = true;
    start_ = {};
    end_ = {};
}
}