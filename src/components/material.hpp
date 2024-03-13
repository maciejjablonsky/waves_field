#pragma once
#include <format>
#include <resource/shaders_manager.hpp>
#include <uniform_buffer.hpp>

namespace wf::components
{
class material
{
  private:
    std::reference_wrapper<resource::shader_program> shader_;
    std::map<std::string, uniform_buffer> uniform_buffers_;

    void create_uniform_buffers_();

  public:
    material(wf::resource::shader_program& shader);
    material(material&& other) noexcept;
    material& operator=(material&& other) noexcept;
    void swap(material& other) noexcept;

    template <typename Self> auto& get_shader(this Self&& self)
    {
        return self.shader_.get();
    }

    void bind();

    void assign_uniform(const std::string& name,
                        std::contiguous_iterator auto begin,
                        std::contiguous_iterator auto end)
    {
        const auto& info = shader_.get().uniform_infos.at(name);
        uniform_buffers_[info.block_name].set(name, begin, end);
    }

    template <typename T>
    void assign_uniform(const std::string& name, T&& value)
    {
        const auto& info = shader_.get().uniform_infos.at(name);
        shader_.get().use();
        uniform_buffers_[info.block_name].set(name, std::forward<T>(value));
    }
};
} // namespace wf::components
