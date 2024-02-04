#pragma once
#include <variant>
#include <glm/glm.hpp>

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
struct change_position : command
{
    glm::vec3 direction;
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
} // namespace input_commands

using input_command = std::variant<input_commands::change_camera_target, input_commands::close_window, input_commands::change_position, input_commands::change_camera_zoom>;


template <typename T>
concept input_commands_view = std::ranges::view<T> &&
                       std::same_as<std::ranges::range_value_t<T>, input_command>;
} // namespace wf::systems