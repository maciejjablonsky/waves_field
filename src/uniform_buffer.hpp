#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <optional>
#include <resource/shaders_manager.hpp>
#include <unordered_map>

namespace wf
{
using namespace std::string_view_literals;
using namespace std::string_literals;

class uniform_buffer : public wf::non_copyable
{
  public:
    enum class mode
    {
        rarely_updated,
        frequently_updated
    };

  private:
    std::unordered_map<std::string_view, size_t> offsets_;
    size_t alignment_{};
    size_t end_offset_{};
    std::optional<uint32_t> ubo_{};
    std::optional<mode> mode_{};

  public:
    void set(mode);
    void bind();
    void configure();
    ~uniform_buffer();

    template <typename T>
    void define(std::string_view name, size_t alignment = alignof(T))
    {
        alignment_     = std::max(alignment_, alignment);
        size_t padding = (alignment_ - (end_offset_ % alignment_)) % alignment_;
        size_t aligned_offset = end_offset_ + padding;
        offsets_.emplace(name, aligned_offset);
        end_offset_ = aligned_offset + sizeof(T);
    }

    void define(std::string_view name,
                const resource::uniform_info& uniform_info);

    template <typename T>
    void set(const std::string& name, T&& value, size_t alignment = alignof(T))
    {
        set(static_cast<std::string_view>(name),
            std::forward<T>(value),
            alignment);
    }

    template <typename T> void set(std::string_view name, T&& value)
    {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsets_.at(name),
                        sizeof(value),
                        std::addressof(value));
    }

    inline void set(std::string_view name, const glm::mat4& matrix)
    {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsets_.at(name),
                        sizeof(matrix),
                        glm::value_ptr(matrix));
    }

    void connect(const resource::shader_program& shader,
                 std::string_view uniform_block_name,
                 uint32_t binding_point = 0) const;
};
} // namespace wf
