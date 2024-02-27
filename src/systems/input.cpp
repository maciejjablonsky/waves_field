#include "input.hpp"
#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <systems/unit_axes.hpp>
#include <utils.hpp>

namespace wf::systems
{
bool wf::systems::input::is_pc_input_used_() const
{
    return true;
}

bool wf::systems::input::is_pc_input_active_() const
{
    return std::holds_alternative<pc_input>(active_system_);
}

bool wf::systems::input::is_xbox_controller_input_used_() const
{
    return false;
}

bool wf::systems::input::is_xbox_controller_input_active_() const
{
    return false;
}

void wf::systems::input::process_cursor_position_(double x_pos, double y_pos)
{
    if (not last_cursor_position_)
    {
        last_cursor_position_.emplace(x_pos, y_pos, 0.f);
    }
    input_commands::change_camera_target command{
        .offset = glm::vec3{x_pos - last_cursor_position_->x,
                            y_pos - last_cursor_position_->y,
                            0.}};
    commands_.push_back(std::move(command));
    commands_.emplace_back(input_commands::recompute_mvp_matrices{});
    last_cursor_position_.emplace(x_pos, y_pos, 0.f);
}

void wf::systems::input::cursor_position_callback_(GLFWwindow* glfw_window,
                                                   double x_pos,
                                                   double y_pos)
{
    auto self = static_cast<input*>(glfwGetWindowUserPointer(glfw_window));
    assert(self && "glfw user pointer wasn't set");
    self->process_cursor_position_(x_pos, y_pos);
}

void wf::systems::input::process_mouse_scroll_(double x_offset, double y_offset)
{
    commands_.emplace_back(input_commands::recompute_mvp_matrices{});
    input_commands::change_camera_zoom c;
    c.offset.x = x_offset;
    c.offset.y = y_offset;
    commands_.emplace_back(c);
}

void wf::systems::input::mouse_scroll_callback_(GLFWwindow* glfw_window,
                                                double x_offset,
                                                double y_offset)
{
    auto self = static_cast<input*>(glfwGetWindowUserPointer(glfw_window));
    assert(self && "glfw user pointer wasn't set");
    self->process_mouse_scroll_(x_offset, y_offset);
}

void wf::systems::input::initialize(gsl::not_null<GLFWwindow*> glfw_window)
{
    glfw_window_ = glfw_window;
    glfwSetWindowUserPointer(glfw_window, this);
    glfwSetCursorPosCallback(glfw_window, input::cursor_position_callback_);
    glfwSetScrollCallback(glfw_window_, input::mouse_scroll_callback_);
    glfwSetKeyCallback(glfw_window, input::key_callback_);
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void wf::systems::input::update(entt::registry& entities)
{
    commands_.clear();
    glfwPollEvents();
    if (is_pc_input_used_() and not is_pc_input_active_())
    {
        active_system_.emplace<pc_input>(gsl::make_not_null(glfw_window_));
    }
    if (is_xbox_controller_input_used_() and
        not is_xbox_controller_input_active_())
    {
        throw unimplemented_error{
            "Xbox controller support not implemented yet"};
    }
}

bool is_in(auto v, std::ranges::range auto r)
{
    return std::ranges::find(r, v) != r.end();
}

std::optional<glm::vec3> direction_from_key(int key)
{
    switch (key)
    {
    case GLFW_KEY_W:
        return -z_unit;
    case GLFW_KEY_S:
        return z_unit;
    case GLFW_KEY_A:
        return -x_unit;
    case GLFW_KEY_D:
        return x_unit;
    case GLFW_KEY_UP:
        return y_unit;
    case GLFW_KEY_DOWN:
        return -y_unit;
    }
    return {};
}

void input::process_key_(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        commands_.emplace_back(
            std::in_place_type<input_commands::close_window>);
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        commands_.emplace_back(
            std::in_place_type<input_commands::toggle_global_clock>);
    }

    if (auto maybe_direction = direction_from_key(key))
    {
        input_commands::change_position change_position_command;
        change_position_command.direction = maybe_direction.value();
        commands_.push_back(change_position_command);
    }
}

void input::key_callback_(
    GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
{
    auto self = static_cast<input*>(glfwGetWindowUserPointer(glfw_window));
    assert(self && "glfw user pointer wasn't set");
    self->process_key_(key, scancode, action, mods);
}

bool input::is_window_open()
{
    if (window_open_)
    {
        auto commands = get_mutable_commands_to_be_executed();
        auto opt_close_window_command =
            std::ranges::find_if(commands, [](const input_command& c) {
                return std::holds_alternative<input_commands::close_window>(c);
            });
        if (commands.end() != opt_close_window_command)
        {
            std::get<input_commands::close_window>(*opt_close_window_command)
                .mark_as_executed();
            window_open_ = false;
        }
    }
    return window_open_;
}
} // namespace wf::systems
