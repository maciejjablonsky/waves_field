#include "clock.hpp"
#include <GLFW/glfw3.h>

namespace wf
{

void clock::tick()
{
    if (running_)
    {
        last_    = current_;
        current_ = std::chrono::high_resolution_clock::now() - skip_;
    }
    else
    {
        skip_ = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - current_);
    }
}

void clock::operator()(wf::systems::input_commands::toggle_global_clock& c)
{
    c.mark_as_executed();
    running_ = not running_;
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
