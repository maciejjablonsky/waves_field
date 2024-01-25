#pragma once
#include <entt/entt.hpp>
#include <glfw_glad.hpp>
#include <gsl/gsl>

namespace wf::systems
{
class pc_input
{
  public:
    pc_input(gsl::not_null<GLFWwindow*> glfw_window);
    void process(entt::registry& entities);
};
}