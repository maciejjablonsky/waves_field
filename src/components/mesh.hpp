#pragma once

#include <boost/function_types/parameter_types.hpp>
#include <boost/functional/hash.hpp>
#include <boost/typeof/typeof.hpp>
#include <components/grid.hpp>
#include <components/render.hpp>
#include <concepts>
#include <entt/entt.hpp>
#include <filesystem>
#include <format>
#include <glm/glm.hpp>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <systems/unit_axes.hpp>
#include <utils.hpp>
#include <vector>

namespace wf
{
// this is default template, to proceed define specialization
template <typename T> struct vertex_attribute_pointer
{
    vertex_attribute_pointer() = delete;
};

template <typename T>
concept has_vertex_definition =
    requires(vertex_attribute_pointer<T> p, uint32_t& attribute_index) {
        {
            p.set_vertex_attribute_pointer(attribute_index)
        };
    } and
    std::same_as<
        typename function_traits<
            decltype(&vertex_attribute_pointer<
                     T>::set_vertex_attribute_pointer)>::template argument<0>,
        uint32_t&>;

} // namespace wf

namespace wf::components
{

class vertex_buffer_layout : wf::non_copyable
{
    uint32_t vao_{};
    uint32_t attribute_index_{};
    std::vector<entt::id_type> hashed_attributes_;

  public:
    vertex_buffer_layout();
    vertex_buffer_layout(vertex_buffer_layout&& other) noexcept;
    vertex_buffer_layout& operator=(vertex_buffer_layout&& other) noexcept;
    void swap(vertex_buffer_layout& other);

    ~vertex_buffer_layout();
    void bind() const;

    template <has_vertex_definition T> void define()
    {
        bind();
        vertex_attribute_pointer<T>::set_vertex_attribute_pointer(
            attribute_index_);
        hashed_attributes_.push_back(entt::type_hash<T>::value());
    }

    template <typename T> bool contains() const
    {
        return std::find_if(std::begin(hashed_attributes_),
                            std::end(hashed_attributes_),
                            [this](entt::id_type id) {
                                return id == entt::type_hash<T>::value();
                            }) != std::end(hashed_attributes_);
    }
};

class mesh : wf::non_copyable
{
  public:
    enum class update_frequency : uint8_t
    {
        rarely,
        often
    };

  private:
    vertex_buffer_layout layout_;

    std::optional<size_t> vertices_count_;
    std::vector<uint32_t> buffers_;

    void check_vertices_count_(size_t new_count);
    template <typename T> void check_vertex_type_in_layout_()
    {
        if (layout_.contains<T>())
        {
            throw std::runtime_error{std::format(
                "mesh already has {} attribute defined!", typeid(T).name())};
        }
    }

  public:
    size_t get_vertices_number() const;
    void bind() const;
    void assign(std::contiguous_iterator auto begin,
                std::contiguous_iterator auto end,
                update_frequency f = update_frequency::rarely)
        requires has_vertex_definition<
            typename std::iterator_traits<decltype(begin)>::value_type>
    {
        using value_type = std::iterator_traits<decltype(begin)>::value_type;
        check_vertex_type_in_layout_<value_type>();
        check_vertices_count_(end - begin);

        auto update_type = [=] {
            switch (f)
            {
            case update_frequency::often:
                return GL_DYNAMIC_DRAW;
            case update_frequency::rarely:
                return GL_STATIC_DRAW;
            default:
                throw std::runtime_error{std::format(
                    "unknown update frequency: {}\n", std::to_underlying(f))};
            }
        }();

        layout_.bind();

        uint32_t new_buffer{};
        glGenBuffers(1, std::addressof(new_buffer));
        buffers_.push_back(new_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, new_buffer);

        auto size_in_bytes = (end - begin) * sizeof(value_type);
        glBufferData(GL_ARRAY_BUFFER,
                     size_in_bytes,
                     std::to_address(begin),
                     update_type);
        layout_.define<value_type>();
    }
};
} // namespace wf::components
