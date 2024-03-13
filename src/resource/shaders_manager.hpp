#pragma once
#include <concepts>
#include <config.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <glfw_glew.hpp>
#include <glm/glm.hpp>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utils.hpp>

namespace wf::resource
{
struct precompiled_shader : private non_copyable
{
    GLuint id{};

    precompiled_shader(GLuint shader_type, const std::string& source_code);

    precompiled_shader(precompiled_shader&&) noexcept;

    ~precompiled_shader() noexcept;
};

struct uniform_info
{
    int32_t location{};
    uint32_t type{};
    int32_t size{};
    std::string block_name{};
    int32_t block_index{};
    uint32_t block_binding{};
    uint32_t offset{};
};

struct shader_program : private non_copyable
{
    GLuint id{};
    std::map<std::string, uniform_info> uniform_infos;

    shader_program(std::forward_iterator auto begin,
                   std::forward_iterator auto end)
    {
        id = glCreateProgram();
        std::for_each(begin, end, [=, this](const precompiled_shader& shader) {
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
        analyze_program_for_uniforms_();
    }

    shader_program(shader_program&&) noexcept;
    shader_program& operator=(shader_program&&) noexcept;
    void swap(shader_program& other) noexcept;

    void set(std::string_view name, const glm::vec3& value);
    void set(std::string_view name, const glm::mat4& value);
    void set(std::string_view name, float value);
    void use() const;

    ~shader_program() noexcept;

  private:
    void analyze_program_for_uniforms_();
};

class shaders_manager
{
  private:
    std::pmr::unordered_map<std::string, shader_program> shaders_;
    std::reference_wrapper<const wf::shaders_config> config_;

  public:
    shaders_manager(const wf::shaders_config&);
    void init();

    auto& get(const std::string& name)
    {
        if (auto it = shaders_.find(name); it != std::end(shaders_))
        {
            return it->second;
        }
        throw std::runtime_error{
            fmt::format("No shader identified by \"{}\"\n", name)};
    }
};

struct uniform_names
{
    static constexpr std::string_view view_matrix       = "u_view";
    static constexpr std::string_view projection_matrix = "u_projection";
    static constexpr std::string_view model_matrix      = "u_model";
    static constexpr std::string_view camera_position   = "u_camera_position";

    static constexpr inline auto names = {
        view_matrix, projection_matrix, model_matrix, camera_position};

    static constexpr std::string_view camera_block = "camera";

    static constexpr inline auto block_names = {camera_block};
};
} // namespace wf::resource
