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

int mesh::get_indices_count() const
{
    return index_buffer_.get_indices_count();
}

void mesh::bind() const
{
    layout_.bind();
    index_buffer_.bind();
}

void mesh::draw()
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

index_buffer::index_buffer()
{
    glGenBuffers(1, std::addressof(ebo_));
}

index_buffer::index_buffer(index_buffer&& other) noexcept
{
    swap(other);
}

index_buffer& index_buffer::operator=(index_buffer&& other) noexcept
{
    if (this != std::addressof(other))
    {
        swap(other);
    }
    return *this;
}

void index_buffer::swap(index_buffer& other) noexcept
{
    std::swap(ebo_, other.ebo_);
    std::swap(indices_count_, other.indices_count_);
}

index_buffer::~index_buffer() noexcept
{
    glDeleteBuffers(1, std::addressof(ebo_));
}

void index_buffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
}

int index_buffer::get_indices_count() const
{
    return indices_count_;
}
} // namespace wf::components
