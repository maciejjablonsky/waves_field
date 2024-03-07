#pragma once
#include <entt/entt.hpp>
#include <glfw_glew.hpp>
#include <gsl/gsl>
#include <ranges>
#include <systems/input_command.hpp>
#include <unordered_map>
#include <vector>

namespace wf::systems
{
enum class key_state : uint8_t
{
    idle,
    pressed,
    active,
    released
};

enum class player_state : uint8_t
{
    stationary,
    moving
};

class pc_input
{
  private:
    std::reference_wrapper<std::vector<systems::input_command>> commands_;
    std::unordered_map<int, key_state> keys_state_;
    player_state player_state_ = player_state::stationary;

    bool is_key_pressed_(int key) const;
    bool is_key_released_(int key) const;

    bool is_key_just_released_(int key) const;

    void process_movement_keys_();

  public:
    pc_input(gsl::not_null<GLFWwindow*> glfw_window,
             std::vector<systems::input_command>& commands);
    void consume_key(int key, int scancode, int action, int mods);
    void update();
};
} // namespace wf::systems
