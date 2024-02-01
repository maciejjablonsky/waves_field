#pragma once
#include <chrono>

namespace wf
{
class clock
{
  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last_;
    std::chrono::time_point<std::chrono::high_resolution_clock> current_;

  public:
    std::chrono::microseconds delta() const;
    std::chrono::microseconds current() const;
    void tick();
};
} // namespace wf
