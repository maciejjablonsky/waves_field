#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "config.hpp"
#include "shader.hpp"
#include "camera.hpp"

namespace wf
{
constexpr auto grid_size    = 100.f;
constexpr auto grid_spacing = 1.f;

class grid
{
  private:
    uint32_t vbo_{};
    uint32_t vao_{};
    
    shader shader_;
    size_t vertices_count_{};
    std::reference_wrapper<const config> config_;
    std::reference_wrapper<camera> camera_;

    std::vector<glm::vec3> make_vertices_buffer_();
    void display_control_cube_();

  public:
    grid(const config & c, camera & camera);
    void show();

    ~grid() noexcept;
};
} // namespace wf