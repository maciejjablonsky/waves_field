#include <components/transform.hpp>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ranges>
#include <systems/camera.hpp>
#include <systems/input_command.hpp>
#include <utils.hpp>

template <> struct fmt::formatter<glm::vec3> : fmt::formatter<std::string>
{
    auto format(const glm::vec3& v, format_context& ctx) const
    {
        auto it = ctx.out();
        it      = std::format_to(it, "[x: {}, y: {}, z: {}]", v.x, v.y, v.z);
        return it;
    }
};

namespace wf::systems
{
void camera::update_matrices_()
{
    auto forward = orientation_ * z_unit;
    auto up      = orientation_ * y_unit;
    view_        = glm::lookAt(position_, position_ + forward, up);
    projection_  = glm::perspective(
        glm::radians(45.f), viewport_.x / viewport_.y, 0.00001f, 100.f);
}

void camera::initialize(const glm::vec3& look_from,
                        const glm::vec3& look_at,
                        const glm::vec2& viewport)
{
    position_    = look_from;
    orientation_ = glm::quatLookAt(glm::normalize(look_at - look_from), y_unit);
    update_matrices_();
    viewport_ = viewport;
}

void camera::operator()(input_commands::change_camera_target& c)
{
    c.mark_as_executed();
    auto sensitive_offset = c.offset * 0.1f;

    float yaw_angle   = sensitive_offset.x;
    float pitch_angle = sensitive_offset.y;

    auto right = orientation_ * x_unit;

    auto yaw_quat   = glm::angleAxis(glm::radians(-yaw_angle), y_unit);
    auto pitch_quat = glm::angleAxis(glm::radians(-pitch_angle), right);
    orientation_    = glm::normalize(yaw_quat * pitch_quat * orientation_);
    update_matrices_();
}


void camera::operator()(input_commands::change_position& c)
{
    c.mark_as_executed();
    float velocity = 0.1;
    position_ += velocity * glm::normalize(orientation_ * c.direction);
    update_matrices_();
}

const glm::mat4& camera::get_view() const
{
    return view_;
}

const glm::mat4& camera::get_projection() const
{
    return projection_;
}
} // namespace wf::systems