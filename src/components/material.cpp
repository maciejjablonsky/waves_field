#include <components/material.hpp>
#include <gl_error.hpp>

namespace wf::components
{
void material::create_uniform_buffers_()
{
}
material::material(wf::resource::shader_program& shader) : shader_{shader}
{
    std::vector<std::pair<std::string, resource::uniform_info>> infos(
        shader.uniform_infos.size());
    std::transform(shader.uniform_infos.begin(),
                   shader.uniform_infos.end(),
                   infos.begin(),
                   [](const auto& pair) { return pair; });
    std::sort(std::begin(infos),
              std::end(infos),
              [](const auto& lhs, const auto& rhs) {
                  if (lhs.second.block_name != rhs.second.block_name)
                  {
                      return lhs.second.block_name < rhs.second.block_name;
                  }
                  else
                  {
                      return lhs.second.offset < rhs.second.offset;
                  }
              });
    for (const auto& [name, info] : infos)
    {
        uniform_buffers_[info.block_name].define(name, info);
    }
    for (auto& [name, buffer] : uniform_buffers_)
    {
        buffer.set(wf::uniform_buffer::mode::rarely_updated);
        buffer.configure();
    }
}

material::material(material&& other) noexcept
    : shader_{std::move(other.shader_)},
      uniform_buffers_{std::move(other.uniform_buffers_)}
{
}

material& material::operator=(material&& other) noexcept
{
    if (this != &other)
    {
        swap(other);
    }
    return *this;
}

void material::swap(material& other) noexcept
{
    using std::swap;
    swap(shader_, other.shader_);
    swap(uniform_buffers_, other.uniform_buffers_);
}

void material::bind()
{
    glUseProgram(shader_.get().id);
    for (auto& [name, uniform_buffer] : uniform_buffers_)
    {
        uniform_buffer.connect(shader_.get(), name);
    }
}
} // namespace wf::components
