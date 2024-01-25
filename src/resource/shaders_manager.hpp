#pragma once
#include <config.hpp>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <utils.hpp>
#include <fmt/format.h>

namespace wf::resource
{
struct precompiled_shader : private non_copyable
{
    GLuint id{};

    precompiled_shader(GLuint shader_type, const std::string& source_code);

    precompiled_shader(precompiled_shader&&) noexcept;

    ~precompiled_shader() noexcept;
};

struct shader_program : private non_copyable
{
    GLuint id{};

    shader_program(std::forward_iterator auto begin,
                   std::forward_iterator auto end)
    {
        id = glCreateProgram();
        std::for_each(begin, end, [=](const precompiled_shader& shader) {
            glAttachShader(id, shader.id);
        });
        glLinkProgram(id);

        int success{};
        glGetProgramiv(id, GL_LINK_STATUS, std::addressof(success));
        if (not success)
        {
            std::string info_log{};
            info_log.resize(1024);
            glGetProgramInfoLog(id, info_log.size(), nullptr, info_log.data());
            throw std::runtime_error{info_log};
        }
    }

    shader_program(shader_program&&) noexcept;
    shader_program& operator=(shader_program&&) noexcept;
    void swap(shader_program& other) noexcept;

    void set(std::string_view name, const glm::vec3& value);
    void set(std::string_view name, const glm::mat4& value);
    void use() ;

    ~shader_program() noexcept;
};

class shaders_manager
{
  private:
    std::pmr::unordered_map<std::string, shader_program> shaders_;
    std::reference_wrapper<const wf::shaders_config> config_;

  public:
    shaders_manager(const wf::shaders_config&);
    void init();

    template <typename Self>
    auto& get(this Self&& self, const std::string& name)
    {
        if (auto it = self.shaders_.find(name); it != std::end(self.shaders_))
        {
            return it->second;
        }
        throw std::runtime_error{
            fmt::format("No shader identified by \"{}\"\n", name)};
    }
};
} // namespace wf::resource