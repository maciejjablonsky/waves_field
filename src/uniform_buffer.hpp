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
    std::unordered_map<std::string, size_t> offsets_;
    size_t alignment_{};
    size_t end_offset_{};
    std::optional<uint32_t> ubo_{};
    std::optional<mode> mode_{};
    std::optional<uint32_t> binding_point_{};
    std::optional<int32_t> block_index_{};

    void set_or_compare_binding_point_(
        const std::optional<uint32_t>& new_binding_point);
    void set_or_compare_block_index_(
        const std::optional<uint32_t>& new_block_index);

  public:
    void set(mode);
    void bind() const;
    void configure();
    ~uniform_buffer();

    void set_binding_point(uint32_t binding_point);

    template <typename T>
    void define(std::string_view name,
                size_t size         = 1,
                size_t alignment    = alignof(T),
                size_t element_size = sizeof(T))
    {
        alignment_     = std::max(alignment_, alignment);
        size_t padding = (alignment_ - (end_offset_ % alignment_)) % alignment_;
        size_t aligned_offset = end_offset_ + padding;
        offsets_.emplace(name, aligned_offset);
        end_offset_ = aligned_offset + element_size * size;
    }

    void define(std::string_view name,
                const resource::uniform_info& uniform_info);

    template <typename... Ts> void set(const std::string& name, Ts&&... ts)
    {
        set(static_cast<std::string_view>(name), std::forward<Ts>(ts)...);
    }

    template <typename T> void set(std::string_view name, T&& value)
    {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsets_.at(std::string(name)),
                        sizeof(value),
                        std::addressof(value));
    }

    inline void set(std::string_view name, const glm::mat4& matrix)
    {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsets_.at(std::string(name)),
                        sizeof(matrix),
                        glm::value_ptr(matrix));
    }

    inline void set(std::string_view name,
                    std::contiguous_iterator auto begin,
                    std::contiguous_iterator auto end)
    {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsets_.at(std::string(name)),
                        std::distance(begin, end) * sizeof(*begin),
                        std::to_address(begin));
    }

    void connect(const resource::shader_program& shader,
                 const std::string& uniform_block_name);
};
} // namespace wf
