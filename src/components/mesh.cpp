#include <components/mesh.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>

namespace wf::components
{
std::vector<mesh_vertex> make_vertex_data(const tinyobj::shape_t& shape,
                                          const tinyobj::attrib_t& attribute)
{
    std::vector<mesh_vertex> vertices;
    vertices.reserve(shape.mesh.indices.size());
    for (const auto& index : shape.mesh.indices)
    {
        mesh_vertex v = {
            .position = {attribute.vertices[3 * index.vertex_index + 0] * 0.5,
                         attribute.vertices[3 * index.vertex_index + 1] * 0.5,
                         attribute.vertices[3 * index.vertex_index + 2] * 0.5}};
        vertices.push_back(v);
    }
    return vertices;
}

mesh::mesh(components::render& render_component,
           const std::filesystem::path& mesh_path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    std::string warns;
    if (not tinyobj::LoadObj(std::addressof(attrib),
                             std::addressof(shapes),
                             std::addressof(materials),
                             std::addressof(warns),
                             std::addressof(err),
                             mesh_path.string().c_str()))
    {
        throw std::runtime_error(err);
    }

    auto cpu_vertex_buffer_data = make_vertex_data(shapes[0], attrib);
    cpu_to_gpu_vertex_buffer(render_component.vao, cpu_vertex_buffer_data);
    render_component.vertices_number = cpu_vertex_buffer_data.size();
}

void mesh::cpu_to_gpu_vertex_buffer(GLuint vao,
                                    const std::vector<mesh_vertex>& cpu_data)
{
    glBindVertexArray(vao);
    if (not vbo_.has_value())
    {
        uint32_t vbo{};
        glGenBuffers(1, std::addressof(vbo));
        vbo_ = vbo;
        glBindBuffer(GL_ARRAY_BUFFER, *vbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     cpu_data.size() * sizeof(mesh_vertex),
                     cpu_data.data(),
                     GL_STATIC_DRAW);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, *vbo_);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        cpu_data.size() * sizeof(mesh_vertex),
                        cpu_data.data());
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
} // namespace wf::components