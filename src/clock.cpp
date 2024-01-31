#include "clock.hpp"

namespace wf
{

void clock::tick()
{
    last_    = current_;
    current_ = std::chrono::steady_clock::now();
}

std::chrono::microseconds clock::delta() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(current_ -
                                                                 last_);
}
std::chrono::microseconds clock::current() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
        current_.time_since_epoch());
}
} // namespace wf
