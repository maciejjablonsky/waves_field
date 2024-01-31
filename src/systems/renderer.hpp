#pragma once
#include <config.hpp>
#include <entt/entt.hpp>
#include <glfw_glad.hpp>
#include <gsl/gsl>
#include <resource/shaders_manager.hpp>

namespace wf::systems
{
class renderer
{
  private:
    GLFWwindow* glfw_window_;
    glm::vec2 viewport_;
    static void framebuffer_size_callback(GLFWwindow* window,
                                          int width,
                                          int height);
    void render_unit_axes_(resource::shaders_manager& shaders_manager,
                           const glm::mat4& view_matrix,
                           const glm::mat4& projection_matrix) const;

    void render_grids_(entt::registry& entities,
                       resource::shaders_manager& shaders_manager,
                       const glm::mat4& view_matrix,
                       const glm::mat4& projection_matrix) const;

  public:
    void initialize(const renderer_config&);
    virtual ~renderer() noexcept;
    const gsl::not_null<const GLFWwindow*> get_glfw_window() const;
    gsl::not_null<GLFWwindow*> get_mutable_glfw_window();
    glm::vec2 get_viewport() const;
    void update(entt::registry& entities,
                resource::shaders_manager& shaders_manager,
                const glm::mat4& view_matrix,
                const glm::mat4& projection_matrix) const;
};
} // namespace wf::systems