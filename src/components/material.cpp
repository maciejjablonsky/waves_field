#include <components/material.hpp>
#include <gl_error.hpp>

namespace wf::components
{
void material::create_uniform_buffers_()
{
}
material::material(wf::resource::shader_program& shader) : shader_{shader}
{
    for (const auto& [name, info] : shader.uniform_infos)
    {
    }
}

void material::bind(uint32_t& binding_point) const
{
    glUseProgram(shader_.get().id);
}
} // namespace wf::components
