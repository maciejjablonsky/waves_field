#include <components/mesh.hpp>
#include <glfw_glew.hpp>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>

namespace wf::components
{
void mesh::check_vertices_count_(size_t new_count)
{
    if (vertices_count_.has_value() and new_count != *vertices_count_)
    {
        throw std::runtime_error{std::format(
            "mesh has already set {} vertices, but trying to set {}\n",
            *vertices_count_,
            new_count)};
    }
    else
    {
        vertices_count_ = new_count;
    }
}

size_t mesh::get_vertices_number() const
{
    return *vertices_count_;
}

void mesh::bind() const
{
    layout_.bind();
}

void vertex_buffer_layout::bind() const
{
    glBindVertexArray(vao_);
}

vertex_buffer_layout::vertex_buffer_layout()
{
    glGenVertexArrays(1, std::addressof(vao_));
}

vertex_buffer_layout::vertex_buffer_layout(
    vertex_buffer_layout&& other) noexcept
{
    swap(other);
}

vertex_buffer_layout& vertex_buffer_layout::operator=(
    vertex_buffer_layout&& other) noexcept
{
    if (this != std::addressof(other))
    {
        swap(other);
    };
    return *this;
}

void vertex_buffer_layout::swap(vertex_buffer_layout& other)
{
    std::swap(vao_, other.vao_);
    std::swap(attribute_index_, other.attribute_index_);
    std::swap(hashed_attributes_, other.hashed_attributes_);
}

vertex_buffer_layout::~vertex_buffer_layout()
{
    glDeleteVertexArrays(1, std::addressof(vao_));
}
} // namespace wf::components
