#pragma once
#include <boost/circular_buffer.hpp>
#include <config.hpp>
#include <entt/entt.hpp>
#include <glfw_glad.hpp>
#include <gsl/gsl>
#include <numeric>
#include <resource/shaders_manager.hpp>

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
    static void framebuffer_size_callback(GLFWwindow* window,
                                          int width,
                                          int height);
    void render_unit_axes_(resource::shaders_manager& shaders_manager,
                           const glm::mat4& view_matrix,
                           const glm::mat4& projection_matrix) const;

    void render_grids_(entt::registry& entities,
                       resource::shaders_manager& shaders_manager,
                       const glm::mat4& view_matrix,
                       const glm::mat4& projection_matrix,
                       const glm::vec3& camera_position) const;
    void init_ft_();

  public:
    void initialize(const renderer_config&);
    virtual ~renderer() noexcept;
    const gsl::not_null<const GLFWwindow*> get_glfw_window() const;
    gsl::not_null<GLFWwindow*> get_mutable_glfw_window();
    glm::vec2 get_viewport() const;
    void update(entt::registry& entities,
                resource::shaders_manager& shaders_manager,
                const glm::mat4& view_matrix,
                const glm::mat4& projection_matrix,
                std::chrono::microseconds delta,
                const glm::vec3& camera_position) const;
    void render_text(wf::resource::shader_program& text_shader,
                     const std::string& text,
                     float x,
                     float y,
                     float scale,
                     glm::vec3 color) const;
};
} // namespace wf::systems
