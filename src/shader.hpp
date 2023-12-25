#pragma once
#include <filesystem>
#include <cstdint>
#include <glm/glm.hpp>

namespace wf
{
 class shader
{
  private:
    uint32_t id_;
  public:
    shader(const std::filesystem::path& vertex_path,
           const std::filesystem::path& fragment_path);
    ~shader() noexcept;
    void use() const;

    void set(std::string_view uniform_name, glm::mat4 matrix);
};

}