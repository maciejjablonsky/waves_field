#include "shader.hpp"
#include "utils.hpp"
#include <glad/glad.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>

namespace wf
{
uint32_t compile_shader(uint32_t shader_type,
                        const std::filesystem::path& source_path)
{
    auto source_code = load_text_from_file(source_path);
    uint32_t id      = glCreateShader(shader_type);
    auto shader_code = source_code.data();
    glShaderSource(id, 1, std::addressof(shader_code), nullptr);
    glCompileShader(id);
    int success{};
    glGetShaderiv(id, GL_COMPILE_STATUS, std::addressof(success));
    if (not success)
    {
        std::string info_log{};
        info_log.resize(1024);
        glGetShaderInfoLog(id, info_log.size(), nullptr, info_log.data());
        throw std::runtime_error{info_log};
    }
    return id;
}

shader::shader(const std::filesystem::path& vertex_path,
               const std::filesystem::path& fragment_path)
{
    auto vertex_shader   = compile_shader(GL_VERTEX_SHADER, vertex_path);
    auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_path);

    id_ = glCreateProgram();
    glAttachShader(id_, vertex_shader);
    glAttachShader(id_, fragment_shader);
    glLinkProgram(id_);

    int success{};
    glGetProgramiv(id_, GL_LINK_STATUS, std::addressof(success));
    if (not success)
    {
        std::string info_log{};
        info_log.resize(1024);
        glGetProgramInfoLog(id_, info_log.size(), nullptr, info_log.data());
        throw std::runtime_error{info_log};
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

shader::~shader() noexcept
{
    glDeleteProgram(id_);
}

void shader::use() const
{
    glUseProgram(id_);
}
void shader::set(std::string_view name, glm::mat4 matrix)
{
    GLint location = glGetUniformLocation(id_, name.data());
    if (location != -1)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    else
    {
        throw std::runtime_error{
            fmt::format("Failed to locate uniform \'{}\'\n", name)};
    }
}
} // namespace wf