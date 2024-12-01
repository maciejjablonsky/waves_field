module;
#include <boost/container/flat_map.hpp>
#include <entt/entt.hpp>
#include <GLFW/glfw3.h>
#include <gsl/pointers>
#include <magic_enum/magic_enum.hpp>
#include <variant>
export module systems.input;
import callback_interfaces;
import systems.pc_input;
import system;
import commands;

namespace wf::systems
{
auto make_glfw_keys_mapping()
{
    boost::container::flat_map<int, wf::systems::key> keys;
    keys[GLFW_KEY_ESCAPE] = systems::key::escape;

    return keys;
}

auto make_glfw_keys_action_mapping()
{
    boost::container::flat_map<int, wf::systems::key_state> states;
    states[GLFW_PRESS]   = systems::key_state::pressed;
    states[GLFW_RELEASE] = systems::key_state::released;
    return states;
}

const auto KEYS_MAPPING       = make_glfw_keys_mapping();
const auto KEYS_STATE_MAPPING = make_glfw_keys_action_mapping();

enum class menu_state
{
    visible,
    hidden
};

export class input
{
  private:
    entt::entity settings_entity_ = entt::null;
    std::variant<pc_input> active_system_{
        std::in_place_type<pc_input>, pc_input::create_info{settings_entity_}};
    gsl::not_null<GLFWwindow*> window_handle_;
    menu_state menu_state = menu_state::visible;

    [[nodiscard]] bool is_pc_input_active_system_() const
    {
        return std::holds_alternative<pc_input>(active_system_);
    }

    void setup_keyboard_callback_();
    static void keyboard_callback_(
        GLFWwindow* window, int key, int scancode, int action, int mods);

  public:
    input(entt::entity settings_entity,
          gsl::not_null<GLFWwindow*> window_handle)
        : settings_entity_(settings_entity), window_handle_(window_handle)
    {
        setup_keyboard_callback_();
    }

    ~input();

    void update(entt::registry& registry)
    {
        std::visit(
            [&](systems::system auto& system) { system.update(registry); },
            active_system_);
    }

    void handle_key(key key, key_state state)
    {
        if (not is_pc_input_active_system_())
        {
            active_system_.emplace<pc_input>(
                pc_input::create_info{settings_entity_});
        }
        auto& system = std::get<pc_input>(active_system_);
        system.consume_key(key, state);
    }
};

void input::setup_keyboard_callback_()
{
    glfwSetWindowUserPointer(window_handle_, this);
    glfwSetKeyCallback(window_handle_, input::keyboard_callback_);
}

void input::keyboard_callback_(
    GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto input_system = static_cast<input*>(glfwGetWindowUserPointer(window));
    assert(input_system and "glfw window user pointer wasn't set");
    input_system->handle_key(KEYS_MAPPING.at(key),
                             KEYS_STATE_MAPPING.at(action));
}
input::~input()
{
    glfwSetWindowUserPointer(window_handle_, nullptr);
}
static_assert(system<input>);
} // namespace wf::systems