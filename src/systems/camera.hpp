#pragma once
#include <components/camera.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ranges>
#include <systems/input_command.hpp>
#include <systems/unit_axes.hpp>
#include <utils.hpp>

namespace wf::systems
{
class camera
{
  private:
    void update_main_camera_(input_commands::change_camera_zoom& command,
                             components::camera& camera);
    void update_main_camera_(input_commands::change_camera_target& command,
                             components::camera& camera);

  public:
    void initialize(const glm::vec3& look_from,
                    const glm::vec3& look_at,
                    entt::registry& entities);

    void update(input_commands_view auto input_commands,
                entt::registry& entities)
    {
        auto cameras_view     = entities.view<components::camera>();
        auto [main_camera, _] = find_main_camera(entities);

        auto process_command = [main_camera, this](input_command& c) {
            std::visit(
                overloaded{[](const auto&) {},
                           [main_camera, this](
                               input_commands::change_camera_target& command) {
                               update_main_camera_(command, main_camera);
                           },
                           [main_camera,
                            this](input_commands::change_camera_zoom& command) {
                               update_main_camera_(command, main_camera);
                           }},
                c);
        };
        std::ranges::for_each(input_commands, process_command);
    }
};
} // namespace wf::systems
