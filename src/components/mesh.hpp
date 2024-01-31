#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <components/render.hpp>

namespace wf::components
{
struct mesh_vertex
{
    glm::vec3 position;
};

class mesh
{
  private:
    uint32_t vbo_;

    void cpu_to_gpu_vertex_buffer(GLuint vao, const std::vector<mesh_vertex>& cpu_data);

  public:
    mesh(components::render& render_component, const std::filesystem::path& mesh_path);
};
} // namespace wf::components