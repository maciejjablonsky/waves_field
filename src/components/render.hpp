#pragma once
#include <glfw_glew.hpp>
#include <mdspan>
#include <optional>
#include <resource/shaders_manager.hpp>
#include <utility>

namespace wf::components
{
struct render
{
    GLuint vao;
    size_t vertices_number;
    std::reference_wrapper<resource::shader_program> shader;

    render(resource::shader_program & shader_program);
    ~render();
};
} // namespace wf::components