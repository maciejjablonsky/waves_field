#pragma once
#include <resource/shaders_manager.hpp>
#include <uniform_buffer.hpp>

namespace wf::components
{
class material
{
  private:
    std::reference_wrapper<resource::shader_program> shader_;
    std::unordered_map<std::string, uniform_buffer> uniform_buffers_;

    void create_uniform_buffers_();

  public:
    material(wf::resource::shader_program& shader);
    material(material&& other) noexcept            = default;
    material& operator=(material&& other) noexcept = default;

    template <typename Self> auto& get_shader(this Self&& self)
    {
        return self.shader_.get();
    }

    void bind(uint32_t& binding_point) const;
};
} // namespace wf::components
