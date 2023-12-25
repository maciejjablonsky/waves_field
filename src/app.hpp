#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <concepts>
#include <fmt/format.h>
#include "config.hpp"

namespace wf
{
class app
{
  private:
    GLFWwindow* glfw_window_;
    void process_input_();

  public:
    app(const config& c = {});
    ~app() noexcept;
};
} // namespace wf