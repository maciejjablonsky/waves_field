#include "camera.hpp"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <fmt/format.h>
#include <algorithm>

namespace wf
{
void camera::log()
{
    fmt::print("camera: [pos: {}, {}, {}][direction: {}, {}, {}][pitch: {}][yaw: {}]\n", position_.x, position_.y, position_.z, front_.x, front_.y, front_.z, pitch_, yaw_);
}
void camera::update_vectors_()
{
    glm::vec3 front{};
    front.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front.y = std::sin(glm::radians(pitch_));
    front.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front_  = glm::normalize(front);
    right_  = glm::normalize(glm::cross(front, world_up_));
    up_     = glm::normalize(glm::cross(right_, front));
}

camera::camera(const config& config, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : config_{config}, position_{position}, world_up_{up}, yaw_{yaw},
      pitch_{pitch}
{
    update_vectors_();
}
glm::mat4 camera::view() const
{
    return glm::lookAt(position_, position_ + front_, up_);
}
glm::mat4 camera::projection() const
{
    return glm::perspective(glm::radians(zoom_), static_cast<float>(config_.get().window().width()) / config_.get().window().height(), 0.1f, 10000.f);
}
void camera::process_keyboard(camera_movement direction, float delta_time)
{
    float velocity = movement_speed_ * delta_time;
    switch (direction)
    {
    case camera_movement::forward:
        position_ += front_ * velocity;
        break;
    case camera_movement::backward:
        position_ -= front_ * velocity;
        break;
    case camera_movement::left:
        position_ -= right_ * velocity;
        break;
    case camera_movement::right:
        position_ += right_ * velocity;
        break;
    case camera_movement::up:
        position_ += glm::cross(right_, front_) * velocity;
        break;
    case camera_movement::down:
        position_ -= glm::cross(right_, front_) * velocity;
        break;
    default:
        throw std::runtime_error{
            fmt::format("Unknown camera movement direction: {}\n", std::to_underlying(direction))};
    }
}
void camera::process_mouse_movement(float xoffset,
                                    float yoffset,
                                    GLboolean constrain_pitch)
{
    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    if (constrain_pitch)
    {
        pitch_ = std::clamp(pitch_, -89.f, 89.f);
    }
    update_vectors_();
}
void camera::process_mouse_scroll(float yoffset)
{
    zoom_ -= static_cast<float>(yoffset);
    zoom_ = std::clamp(zoom_, 1.f, 45.f);
}
}