#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include "config.hpp"

namespace wf
{
enum camera_movement : uint8_t
{
    forward,
    backward,
    left,
    right,
    up,
    down,
};

class camera
{
  private:
    glm::vec3 position_;
    glm::vec3 front_ = {0.f, 0.f, -1.f};
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 world_up_;
    float yaw_   = -90.f;
    float pitch_ = 0.f;

    float movement_speed_    = 2.5f;
    float mouse_sensitivity_ = 0.1f;
    float zoom_              = 45.f;
    std::reference_wrapper<const config> config_;

    void update_vectors_();

  public:
    camera(const config& config, glm::vec3 position = {},
           glm::vec3 up       = glm::vec3(0.f, 1.f, 0.f),
           float yaw          = -90.f,
           float pitch        = 0.f);
    glm::mat4 view() const;
    glm::mat4 projection() const;
    void log();

    void process_keyboard(camera_movement direction, float delta_time);
    void process_mouse_movement(float xoffset,
                                float yoffset,
                                GLboolean constrain_pitch = true);
    void process_mouse_scroll(float yoffset);
};
} // namespace wf