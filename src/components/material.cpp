#include <components/material.hpp>
#include <gl_error.hpp>

namespace wf::components
{
void material::create_uniform_buffers_()
{
}
material::material(const wf::resource::shader_program& shader) : shader_{shader}
{
}

const resource::shader_program& material::get_shader() const
{
    return shader_.get();
}

void material::bind() const
{
    glUseProgram(shader_.get().id);
}
} // namespace wf::components
