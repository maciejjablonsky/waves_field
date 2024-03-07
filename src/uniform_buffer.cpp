#include <stdexcept>
#include <uniform_buffer.hpp>

namespace wf
{
void uniform_buffer::set(mode m)
{
    mode_ = m;
}

void uniform_buffer::bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, *ubo_);
}
void uniform_buffer::configure()
{
    uint32_t ubo{};
    glGenBuffers(1, &ubo);
    ubo_ = ubo;

    bind();
    auto usage = [this] {
        if (not mode_.has_value())
        {
            throw std::runtime_error{
                "uniform buffer doesn't have usage mode defined!"};
        }
        switch (*mode_)
        {
        case mode::rarely_updated:
            return GL_STATIC_DRAW;
        case mode::frequently_updated:
            return GL_DYNAMIC_DRAW;
        default:
            std::unreachable();
        }
    }();
    glBufferData(GL_UNIFORM_BUFFER, end_offset_, nullptr, usage);
}
uniform_buffer::~uniform_buffer()
{
    if (ubo_.has_value())
    {
        glDeleteBuffers(1, std::addressof(*ubo_));
    }
}

void uniform_buffer::define(std::string_view name,
                            const resource::uniform_info& uniform_info)
{
    switch (uniform_info.type)
    {
    case GL_FLOAT_MAT4:
        define<glm::mat4>(name);
        break;
    case GL_FLOAT_VEC3:
        define<glm::vec3>(name);
        break;
    default:
        throw std::runtime_error{fmt::format(
            "unknown uniform type to define \'{}\' for \'name\' uniform!",
            uniform_info.type)};
    }
}

void uniform_buffer::connect(const resource::shader_program& shader,
                             std::string_view uniform_block_name,
                             uint32_t binding_point) const
{
    uint32_t block_index =
        glGetUniformBlockIndex(shader.id, uniform_block_name.data());
    if (block_index == GL_INVALID_INDEX)
    {
        throw std::runtime_error{fmt::format(
            "\'{}\' is not valid uniform block name for current shader!",
            uniform_block_name)};
    }
    glUniformBlockBinding(shader.id, block_index, binding_point);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, *ubo_);
}
} // namespace wf