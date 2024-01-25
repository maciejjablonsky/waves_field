#pragma once
#include <entt/entt.hpp>
#include <glfw_glad.hpp>
#include <glm/glm.hpp>
#include <gsl/gsl>
#include <optional>
#include <ranges>
#include <systems/input_command.hpp>
#include <systems/pc_input.hpp>
#include <variant>
#include <vector>

namespace wf::systems
{
class input
{
  private:
    GLFWwindow* glfw_window_;
    std::variant<std::monostate, pc_input> active_system_{};
    std::vector<systems::input_command> commands_{};
    std::optional<glm::vec3> last_cursor_position_;
    bool window_open_ = true;

    bool is_pc_input_used_() const;
    bool is_pc_input_active_() const;
    bool is_xbox_controller_input_used_() const;
    bool is_xbox_controller_input_active_() const;

    void process_cursor_position_(double x_pos, double y_pos);

    static void cursor_position_callback_(GLFWwindow* glfw_window,
                                          double x_pos,
                                          double y_pos);

    void process_mouse_scroll_(double x_offset, double y_offset);
    static void mouse_scroll_callback_(GLFWwindow* glfw_window,
                                       double x_offset,
                                       double y_offset);

    void process_key_(int key, int scancode, int action, int mods);
    static void key_callback_(
        GLFWwindow* glfw_window, int key, int scancode, int action, int mods);

  public:
    void initialize(gsl::not_null<GLFWwindow*> glfw_window);
    void update(entt::registry& entities);

    input_commands_view auto get_mutable_commands_to_be_executed()
    {
        return commands_ | std::views::filter([](const input_command& c) {
                   return std::visit(
                       [](const auto& c) { return not c.executed(); }, c);
               });
    }
    bool is_window_open();
};
} // namespace wf::systems