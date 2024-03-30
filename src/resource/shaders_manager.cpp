#include <algorithm>
#include <execution>
#include <filesystem>
#include <fmt/std.h>
#include <glm/gtc/type_ptr.hpp>
#include <print>
#include <ranges>
#include <resource/shaders_manager.hpp>
#include <unordered_set>
#include <utils.hpp>

namespace wf::resource
{
std::pmr::unordered_set<std::string> collect_shader_names(
    const std::filesystem::path& shaders_directory)
{
    std::pmr::unordered_set<std::string> shaders_names{};
    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(shaders_directory))
    {
        if (not entry.is_directory())
        {
            auto relative_path =
                std::filesystem::relative(entry.path(), shaders_directory);
            auto path_without_extension =
                relative_path.parent_path() / relative_path.stem();

            shaders_names.insert(path_without_extension.generic_string());
        }
    }
    return shaders_names;
}

auto find_matching_files(const std::filesystem::path& base_directory,
                         const std::filesystem::path& pattern_to_match)
{
    auto base_name = pattern_to_match.stem().string();

    std::vector<std::filesystem::path> matching_files{};

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator{base_directory})
    {
        if (not entry.is_directory())
        {
            auto relative_path =
                entry.path().lexically_relative(base_directory);
            auto path_without_extension =
                relative_path.parent_path() / relative_path.stem();

            if (path_without_extension == pattern_to_match)
            {
                matching_files.push_back(entry.path());
            }
        }
    }
    return matching_files;
}

GLuint get_shader_type(const std::filesystem::path& shader_path)
{
    auto extension = shader_path.extension();
    std::unordered_map<std::string, uint32_t> ext_to_type{
        {".vert", GL_VERTEX_SHADER},
        {".frag", GL_FRAGMENT_SHADER},
        {".geometry", GL_GEOMETRY_SHADER}};
    if (auto it = ext_to_type.find(extension.string());
        it != std::end(ext_to_type))
    {
        return it->second;
    }
    throw std::runtime_error{fmt::format(
        "Can't match shader type to file {}. Unknown extension {}\n",
        shader_path,
        extension)};
}

precompiled_shader::precompiled_shader(GLuint shader_type,
                                       const std::string& source_code)
{
    id               = glCreateShader(shader_type);
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
}

precompiled_shader::precompiled_shader(precompiled_shader&& other) noexcept
    : id{other.id}
{
    other.id = {};
}

precompiled_shader::~precompiled_shader() noexcept
{
    if (id)
    {
        glDeleteShader(id);
    }
}

shader_program::shader_program(shader_program&& other) noexcept
    : id{other.id}, uniform_infos{std::move(other.uniform_infos)}
{
    other.id = {};
}

shader_program& shader_program::operator=(shader_program&& other) noexcept
{
    if (this != std::addressof(other))
    {
        shader_program temp(std::move(other));
        swap(temp);
    }
    return *this;
}

void shader_program::swap(shader_program& other) noexcept
{
    std::swap(id, other.id);
}

void shader_program::set(std::string_view name, const glm::vec3& value)
{
    auto location = glGetUniformLocation(id, name.data());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void shader_program::set(std::string_view name, const glm::mat4& value)
{
    auto location = glGetUniformLocation(id, name.data());
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
}

void shader_program::set(std::string_view name, float value)
{
    auto location = glGetUniformLocation(id, name.data());
    glUniform1f(location, value);
}

void shader_program::use() const
{
    glUseProgram(id);
}

shader_program::~shader_program() noexcept
{
    if (id)
    {
        glDeleteProgram(id);
    }
}

std::string trim_uniform_name(std::string name)
{
    // auto pos = name.find('[');
    // if (pos != std::string::npos)
    //{
    //     return std::string{name.substr(0, pos)};
    // }
    return std::string{name};
}

void shader_program::analyze_program_for_uniforms_()
{
    int32_t uniforms{};
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, std::addressof(uniforms));
    std::vector<uint32_t> indices(uniforms);
    std::iota(std::begin(indices), std::end(indices), 0);
    std::vector<int32_t> block_indices(uniforms);
    glGetActiveUniformsiv(id,
                          uniforms,
                          indices.data(),
                          GL_UNIFORM_BLOCK_INDEX,
                          block_indices.data());
    std::vector<int32_t> offsets(uniforms);
    glGetActiveUniformsiv(
        id, uniforms, indices.data(), GL_UNIFORM_OFFSET, offsets.data());

    for (int i = 0; i < uniforms; ++i)
    {
        int32_t name_length{};
        uniform_info info;
        std::string name(256, '\0');
        glGetActiveUniform(id,
                           i,
                           name.size(),
                           std::addressof(name_length),
                           std::addressof(info.size),
                           std::addressof(info.type),
                           name.data());
        name.resize(name_length);
        std::print("Uniform name: {}, ", name);

        if (block_indices[i] != -1)
        {
            int32_t name_length{};
            glGetActiveUniformBlockiv(id,
                                      block_indices[i],
                                      GL_UNIFORM_BLOCK_NAME_LENGTH,
                                      std::addressof(name_length));
            std::string blockName(name_length, '\0');
            glGetActiveUniformBlockName(
                id, block_indices[i], name_length, nullptr, blockName.data());
            blockName.resize(name_length);

            std::print("Uniform block name: {}", blockName);
            info.block_name  = trim_uniform_name(blockName);
            info.block_index = block_indices[i];
            GLint uniformBlockBinding;
            glGetActiveUniformBlockiv(id,
                                      info.block_index,
                                      GL_UNIFORM_BLOCK_BINDING,
                                      &uniformBlockBinding);
            info.block_binding = uniformBlockBinding;
            info.offset        = offsets[i];
            name               = trim_uniform_name(name);
            if (!wf::is_in(name, resource::uniform_names::names))
            {
                uniform_infos[name] = info;
            }
        }
        std::println("");
    }

    // std::sort(std::begin(uniform_infos),
    //           std::end(uniform_infos),
    //           [](const auto& lhs, const auto& rhs) {
    //               if (lhs.second.block_name != rhs.second.block_name)
    //               {
    //                   return lhs.second.block_name < rhs.second.block_name;
    //               }
    //               return lhs.second.offset < rhs.second.offset;
    //           });
}

std::pair<std::string, shader_program> make_shader_program(
    const std::filesystem::path& shaders_base_directory,
    const std::filesystem::path& target)
{
    auto shader_files = find_matching_files(shaders_base_directory, target);
    std::pmr::vector<precompiled_shader> precompiled_shaders;
    std::transform(std::begin(shader_files),
                   std::end(shader_files),
                   std::back_inserter(precompiled_shaders),
                   [](const std::filesystem::path& shader_source_path) {
                       return precompiled_shader{
                           get_shader_type(shader_source_path),
                           load_text_from_file(shader_source_path)};
                   });

    // foreach shader source file
    // - check shader type based on extension
    // - if known then read source code and create precompiled_shader
    // - else throw exception
    // when all shader files are compiled
    // create shader program
    // return {shader_name, shader_program_id}
    // precompiled_shaders are deleted when going out of scope (shader
    // program is left)
    return {target.generic_string(),
            shader_program{std::begin(precompiled_shaders),
                           std::end(precompiled_shaders)}};
}

std::pmr::unordered_map<std::string, shader_program> make_shaders_programs(
    const std::filesystem::path& shaders_base_directory)
{
    auto names = collect_shader_names(shaders_base_directory);
    std::pmr::vector<std::pair<std::string, shader_program>> shaders{};
    std::transform(
        std::begin(names),
        std::end(names),
        std::back_inserter(shaders),
        [&shaders_base_directory, &shaders](const std::string& name) {
            return make_shader_program(shaders_base_directory, name);
        });

    std::pmr::unordered_map<std::string, shader_program> programs;
    for (auto&& program : shaders)
    {
        programs.emplace(program.first, std::move(program.second));
    }

    return programs;
}

shaders_manager::shaders_manager(const wf::shaders_config& config)
    : config_{config}
{
}

void shaders_manager::init()
{
    shaders_ = make_shaders_programs(config_.get().source_directory());
}
} // namespace wf::resource
