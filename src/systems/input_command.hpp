#pragma once
#include <glm/glm.hpp>
#include <variant>

namespace wf::systems
{
namespace input_commands
{
struct command
{
  private:
    bool executed_ = false;

  public:
    [[nodiscard]] constexpr bool executed() const
    {
        return executed_;
    }

    void mark_as_executed()
    {
        executed_ = true;
    }
};
struct change_player_velocity_direction : command
{
    glm::vec3 direction;
};

struct stop_player : command
{
};

struct change_camera_target : command
{
    glm::vec3 offset;
};

struct close_window : command
{
};

struct change_camera_zoom : command
{
    glm::vec2 offset;
};

struct toggle_global_clock : command
{
};

struct recompute_mvp_matrices : command
{
};
} // namespace input_commands

using input_command = std::variant<input_commands::change_camera_target,
                                   input_commands::close_window,
                                   input_commands::change_player_velocity_direction,
                                   input_commands::change_camera_zoom,
                                   input_commands::toggle_global_clock,
                                   input_commands::recompute_mvp_matrices,
                                   input_commands::stop_player>;

template <typename T>
concept input_commands_view =
    std::ranges::view<T> &&
    std::same_as<std::ranges::range_value_t<T>, input_command>;
} // namespace wf::systems
