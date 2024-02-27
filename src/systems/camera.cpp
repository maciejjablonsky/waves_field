#include <components/camera.hpp>
#include <components/transform.hpp>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ranges>
#include <systems/camera.hpp>
#include <systems/input_command.hpp>
#include <utils.hpp>

namespace wf::systems
{
void camera::update_main_camera_(input_commands::change_camera_zoom& command,
                                 components::camera& camera)
{
    command.mark_as_executed();
    auto zoom = camera.get_zoom();
    zoom -= command.offset.y;
    zoom = std::clamp(zoom, 0.f, 45.f);
    camera.update_zoom(zoom);
}

void camera::update_main_camera_(input_commands::change_camera_target& command,
                                 components::camera& camera)
{
    command.mark_as_executed();
    auto sensitive_offset = command.offset * 0.1f;

    float yaw_angle   = sensitive_offset.x;
    float pitch_angle = sensitive_offset.y;

    const auto& orientation = camera.get_orientation();
    auto right              = orientation * x_unit;

    auto yaw_quat   = glm::angleAxis(glm::radians(-yaw_angle), y_unit);
    auto pitch_quat = glm::angleAxis(glm::radians(-pitch_angle), right);
    camera.update_orientation(
        glm::normalize(yaw_quat * pitch_quat * orientation));
}

void camera::initialize(const glm::vec3& look_from,
                        const glm::vec3& look_at,
                        entt::registry& entities)
{
    auto main_camera_entity = entities.create();
    auto& transform_component =
        entities.emplace<components::transform>(main_camera_entity);
    transform_component.position = look_from;

    auto& camera_component =
        entities.emplace<components::camera>(main_camera_entity);
    camera_component.set_as_main();
    camera_component.update_orientation(
        glm::quatLookAt(glm::normalize(look_at - look_from), y_unit));
}
} // namespace wf::systems
