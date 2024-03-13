#include <stdexcept>
#include <uniform_buffer.hpp>

namespace wf
{
void uniform_buffer::set_or_compare_binding_point_(
    const std::optional<uint32_t>& new_binding_point)
{
    if (binding_point_.has_value() and new_binding_point.has_value() and
        binding_point_.value() != new_binding_point.value())
    {
        throw std::runtime_error{std::format(
            "assigned binding point {} differs from block binding point {}!",
            binding_point_.value(),
            new_binding_point.value())};
    }
    else if (not binding_point_.has_value())
    {
        set_binding_point(new_binding_point.value());
    }
}
void uniform_buffer::set_or_compare_block_index_(
    const std::optional<uint32_t>& new_block_index)
{
    if (block_index_.has_value() and new_block_index.has_value() and
        block_index_.value() != new_block_index.value())
    {
        throw std::runtime_error{
            std::format("assigned block index {} differs from block index {}!",
                        block_index_.value(),
                        new_block_index.value())};
    }
    else if (not block_index_.has_value())
    {
        block_index_ = new_block_index;
    }
}
void uniform_buffer::set(mode m)
{
    mode_ = m;
}

void uniform_buffer::bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, *ubo_);
}
void uniform_buffer::configure()
{
    uint32_t ubo{};
    glGenBuffers(1, &ubo);
    ubo_ = ubo;

    if (not binding_point_.has_value())
    {
        throw std::runtime_error{"uniform buffer doesn't have binding point "
                                 "defined!"};
    }

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

void uniform_buffer::set_binding_point(uint32_t binding_point)
{
    binding_point_ = binding_point;
}

void uniform_buffer::define(std::string_view name,
                            const resource::uniform_info& uniform_info)
{
    set_or_compare_binding_point_(uniform_info.block_binding);
    set_or_compare_block_index_(uniform_info.block_index);

    switch (uniform_info.type)
    {
    case GL_FLOAT_MAT4:
        define<glm::mat4>(name, uniform_info.size, 16);
        break;
    case GL_FLOAT_VEC3:
        if (uniform_info.size == 1)
        {
            define<glm::vec3>(name, uniform_info.size, 16);
        }
        else
        {
            define<glm::vec3>(name, uniform_info.size, 16, 16);
        }
        break;
    case GL_FLOAT_VEC4:
        define<glm::vec4>(name, uniform_info.size, 16);
        break;
    case GL_FLOAT_VEC2:
        define<glm::vec2>(name, uniform_info.size, 4);
        break;
    case GL_FLOAT:
        if (uniform_info.size == 1)
        {
            define<float>(name, uniform_info.size, 4);
        }
        else
        {
            define<float>(name, uniform_info.size, 16, 16);
        }
        break;
    default:
        throw std::runtime_error{fmt::format(
            "unknown uniform type to define \'{}\' for \'name\' uniform!",
            uniform_info.type)};
    }
}

void uniform_buffer::connect(const resource::shader_program& shader,
                             const std::string& uniform_block_name)
{
    shader.use();
    if (not block_index_.has_value())
    {
        block_index_ =
            glGetUniformBlockIndex(shader.id, uniform_block_name.data());
        if (*block_index_ == GL_INVALID_INDEX)
        {
            throw std::runtime_error{fmt::format(
                "\'{}\' is not valid uniform block name for current shader!",
                uniform_block_name)};
        }
    }
    glUniformBlockBinding(shader.id, *block_index_, *binding_point_);
    glBindBufferBase(GL_UNIFORM_BUFFER, *binding_point_, *ubo_);
}
} // namespace wf
