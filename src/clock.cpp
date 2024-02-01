#include "clock.hpp"
#include <GLFW/glfw3.h>

namespace wf
{

void clock::tick()
{
    last_    = current_;
    current_ = std::chrono::high_resolution_clock::now();
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
