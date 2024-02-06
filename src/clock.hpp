#pragma once
#include <algorithm>
#include <chrono>
#include <ranges>
#include <systems/input_command.hpp>

namespace wf
{
class clock
{
  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> last_;
    std::chrono::time_point<std::chrono::high_resolution_clock> current_;
    std::chrono::microseconds skip_{};
    bool running_ = true;

  public:
    std::chrono::microseconds delta() const;
    std::chrono::microseconds current() const;
    void operator()(wf::systems::input_commands::toggle_global_clock& c);
    void operator()(auto&&){
        // match any command and ignore
    };

    void tick();
    void update(wf::systems::input_commands_view auto commands)
    {
        std::ranges::for_each(commands, [this](wf::systems::input_command& c) {
            std::visit(*this, c);
        });
    }
};
} // namespace wf
