#include <array>
#include <boost/container/static_vector.hpp>
#include <fmt/format.h>
#include <glfw_glew.hpp>
#include <glm/glm.hpp>
#include <numeric>
#include <systems/pc_input.hpp>
#include <systems/unit_axes.hpp>
#include <utils.hpp>

namespace wf::systems
{

bool pc_input::is_key_pressed_(int key) const
{
    if (keys_state_.contains(key))
    {
        auto state = keys_state_.at(key);
        return state == key_state::pressed or state == key_state::active;
    }
    return false;
}
bool pc_input::is_key_released_(int key) const
{
    if (keys_state_.contains(key))
    {
        auto state = keys_state_.at(key);
        return state == key_state::released or state == key_state::idle;
    }
    return true;
}

bool pc_input::is_key_just_released_(int key) const
{
    if (keys_state_.contains(key))
    {
        auto state = keys_state_.at(key);
        return state == key_state::released;
    }
    return false;
}

pc_input::pc_input(gsl::not_null<GLFWwindow*> glfw_window,
                   std::vector<systems::input_command>& commands)
    : commands_{commands}
{
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

void pc_input::process_movement_keys_()
{
    constexpr std::array movement_keys = {GLFW_KEY_W,
                                          GLFW_KEY_S,
                                          GLFW_KEY_A,
                                          GLFW_KEY_D,
                                          GLFW_KEY_UP,
                                          GLFW_KEY_DOWN};
    std::set<int> pressed_keys{};
    for (auto key : movement_keys)
    {
        if (is_key_pressed_(key))
        {
            pressed_keys.insert(key);
        }
    }
    auto remove_opposite_keys = [&pressed_keys](int key_a, int key_b) {
        if (pressed_keys.contains(key_a) and pressed_keys.contains(key_b))
        {
            pressed_keys.erase(key_a);
            pressed_keys.erase(key_b);
        }
    };
    remove_opposite_keys(GLFW_KEY_W, GLFW_KEY_S);
    remove_opposite_keys(GLFW_KEY_A, GLFW_KEY_D);
    remove_opposite_keys(GLFW_KEY_UP, GLFW_KEY_DOWN);
    if (not pressed_keys.empty())
    {
        player_state_            = player_state::moving;
        auto resultant_direction = std::transform_reduce(
            std::begin(pressed_keys),
            std::end(pressed_keys),
            glm::vec3{},
            std::plus{},
            [](int key) { return direction_from_key(key).value(); });
        input_commands::change_player_velocity_direction command;
        command.direction = glm::normalize(resultant_direction);
        commands_.get().push_back(input_commands::recompute_mvp_matrices{});
        commands_.get().push_back(command);
    }
    else if (player_state_ == player_state::moving)
    {
        player_state_ = player_state::stationary;
        commands_.get().push_back(input_commands::stop_player{});
    }
}

void pc_input::consume_key(int key, int scancode, int action, int mods)
{
    switch (action)
    {
    case GLFW_PRESS:
        keys_state_[key] = key_state::pressed;
        break;
    case GLFW_RELEASE:
        keys_state_[key] = key_state::released;
        break;
    default:
        break;
    }
}

void pc_input::update()
{
    if (is_key_pressed_(GLFW_KEY_ESCAPE))
    {
        commands_.get().emplace_back(
            std::in_place_type<input_commands::close_window>);
    }
    if (is_key_pressed_(GLFW_KEY_SPACE))
    {
        commands_.get().emplace_back(
            std::in_place_type<input_commands::toggle_global_clock>);
    }
    process_movement_keys_();

    auto update_key = [](key_state state) {
        switch (state)
        {
        case key_state::pressed:
            return key_state::active;
        case key_state::released:
            return key_state::idle;
        default:
            return state;
        }
    };

    for (auto& [key, value] : keys_state_)
    {
        value = update_key(value);
    }
}

} // namespace wf::systems
