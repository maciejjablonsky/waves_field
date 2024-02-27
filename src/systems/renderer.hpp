#pragma once
#include <boost/circular_buffer.hpp>
#include <clock.hpp>
#include <components/camera.hpp>
#include <components/transform.hpp>
#include <config.hpp>
#include <entt/entt.hpp>
#include <glfw_glad.hpp>
#include <gsl/gsl>
#include <numeric>
#include <resource/shaders_manager.hpp>
#include <systems/input_command.hpp>

namespace wf::systems
{
struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

struct perf_meter
{
    float sum_  = 0.f;
    int frames_ = 0.f;
    float fps_{};
    std::chrono::microseconds time_{};
    float update_step_ = 0.f;

    void add(std::chrono::microseconds t)
    {
        sum_ += t.count();
        update_step_ += t.count() / 1e6;
        ++frames_;

        if (update_step_ > 1.f)
        {
            auto avg     = sum_ / frames_;
            time_        = std::chrono::microseconds{static_cast<int>(avg)};
            fps_         = 1e6 / avg;
            sum_         = 0.f;
            frames_      = 0;
            update_step_ = 0.f;
        }
    }

    std::chrono::microseconds time() const

    {
        return time_;
    }

    float fps() const
    {
        return fps_;
    }
};

class renderer
{
  private:
    GLFWwindow* glfw_window_;
    glm::vec2 viewport_;
    uint32_t text_vao_;
    uint32_t text_vbo_;
    std::map<char, Character> Characters;
    mutable perf_meter perf_;
    glm::mat4 view_matrix_;
    glm::mat4 projection_matrix_;
    wf::optional_ref<const clock> clock_;
    static void framebuffer_size_callback(GLFWwindow* window,
                                          int width,
                                          int height);
    void render_unit_axes_(resource::shaders_manager& shaders_manager,
                           const glm::mat4& view_matrix,
                           const glm::mat4& projection_matrix) const;

    void render_grids_(
        entt::registry& entities,
        resource::shaders_manager& shaders_manager,
        const components::transform& main_camera_transform) const;
    void render_performance_(resource::shaders_manager& shaders_manager);
    void init_ft_();
    void process_input_commands_(input_commands_view auto commands,
                                 const components::camera& main_camera,
                                 const components::transform& camera_transform);
    void recompute_vp_matrices_(const components::camera& camera,
                                const components::transform& camera_transform);

  public:
    void initialize(const renderer_config&,
                    const clock& clock,
                    entt::registry& entities);
    virtual ~renderer() noexcept;
    const gsl::not_null<const GLFWwindow*> get_glfw_window() const;
    gsl::not_null<GLFWwindow*> get_mutable_glfw_window();
    glm::vec2 get_viewport() const;
    void update(input_commands_view auto commands,
                entt::registry& entities,
                resource::shaders_manager& shaders_manager);
    void render_text(wf::resource::shader_program& text_shader,
                     const std::string& text,
                     float x,
                     float y,
                     float scale,
                     glm::vec3 color) const;
};

void renderer::update(input_commands_view auto commands,
                      entt::registry& entities,
                      resource::shaders_manager& shaders_manager)
{
    const auto& [main_camera, main_camera_transform] =
        find_main_camera(entities);
    process_input_commands_(commands, main_camera, main_camera_transform);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render_performance_(shaders_manager);
    // display_control_cube(shaders_manager, view_matrix, projection_matrix);
    render_unit_axes_(shaders_manager, view_matrix_, projection_matrix_);
    render_grids_(entities, shaders_manager, main_camera_transform);

    glfwSwapBuffers(glfw_window_);
}

void renderer::process_input_commands_(
    input_commands_view auto commands,
    const components::camera& main_camera,
    const components::transform& main_camera_transform)
{
    bool recompute_vp_matrices = false;
    std::ranges::for_each(commands, [&recompute_vp_matrices](input_command& c) {
        if (std::holds_alternative<input_commands::recompute_mvp_matrices>(c))
        {
            std::get<input_commands::recompute_mvp_matrices>(c)
                .mark_as_executed();
            recompute_vp_matrices = true;
        }
    });
    if (recompute_vp_matrices)
    {
        recompute_vp_matrices_(main_camera, main_camera_transform);
    }
}
} // namespace wf::systems
