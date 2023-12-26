#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "camera.hpp"
#include "config.hpp"
#include <concepts>
#include <fmt/format.h>

namespace wf
{
class clock
{
  private:
    float previous_{};
    float current_{};
  public:
    float delta() const;
    void tick();
};

class app
{
  private:
    GLFWwindow* glfw_window_;
    camera camera_;

    clock clock_;
    float last_x_{};
    float last_y_{};
    bool first_frame_ = true;

    void process_input_();

  public:
    app(const config& c = {});
    ~app() noexcept;
    void mouse_callback(double xpos, double ypos);
    void scroll_callback(double xoffset, double yoffset);
};
} // namespace wf