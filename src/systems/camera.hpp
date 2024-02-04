#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ranges>
#include <systems/input_command.hpp>
#include <systems/unit_axes.hpp>

namespace wf::systems
{
class camera
{
  private:
    glm::vec3 position_;
    glm::quat orientation_;
    glm::mat4 view_;
    glm::mat4 projection_;
    glm::vec2 viewport_;
    float zoom_{45.f};

    void update_matrices_();
    void update_vectors_();

  public:
    void initialize(const glm::vec3& look_from,
                    const glm::vec3& look_at,
                    const glm::vec2& viewport);
    void operator()(input_commands::change_camera_target& c);
    void operator()(input_commands::change_position& c);
    void operator()(input_commands::change_camera_zoom& c);
    void operator()(const auto& any_command)
    {
        // empty visitor for broadcasted commands
    }
    void update(input_commands_view auto input_commands)
    {
        std::ranges::for_each(
            input_commands, [this](input_command& c) { std::visit(*this, c); });
    }
    const glm::mat4& get_view() const;
    const glm::mat4& get_projection() const;
};
} // namespace wf::systems