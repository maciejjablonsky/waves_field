#pragma once
#include <resource/shaders_manager.hpp>
#include <uniform_buffer.hpp>

namespace wf::components
{
class material
{
  private:
    std::reference_wrapper<const resource::shader_program> shader_;
    std::unordered_map<std::string, uniform_buffer> uniform_buffers_;

    void create_uniform_buffers_();

  public:
    material(const wf::resource::shader_program& shader);
    material(material&& other) noexcept            = default;
    material& operator=(material&& other) noexcept = default;

    const resource::shader_program& get_shader() const;

    void bind() const;
};
} // namespace wf::components
