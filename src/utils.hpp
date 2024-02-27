#pragma once
#include <concepts>
#include <cstdio>
#include <filesystem>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <source_location>
#include <string>

namespace wf
{

void log(const std::string& message,
         const std::source_location& loc = std::source_location::current());

std::string load_text_from_file(const std::filesystem::path& path);

struct non_copyable
{
    non_copyable(const non_copyable&)            = delete;
    non_copyable& operator=(const non_copyable&) = delete;
    non_copyable()                               = default;
    non_copyable(non_copyable&&)                 = default;
    non_copyable& operator=(non_copyable&&)      = default;
};
static_assert(std::default_initializable<non_copyable>);
static_assert(not std::copy_constructible<non_copyable>);
static_assert(std::move_constructible<non_copyable>);

struct scoped_file
{
  private:
    std::FILE* fp_;

  public:
    scoped_file(auto&&... args)
    {
        fp_ = std::fopen(std::forward<decltype(args)>(args)...);
    }
    ~scoped_file();
    operator std::FILE*();
    operator bool();
};

class unimplemented_error : public std::exception
{
  private:
    std::string message_;

  public:
    unimplemented_error(const std::string& message);
    const char* what() const;
};

template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};

template <typename T>
using optional_ref = std::optional<std::reference_wrapper<T>>;
} // namespace wf

template <> struct fmt::formatter<glm::vec3> : fmt::formatter<std::string>
{
    auto format(const glm::vec3& v, format_context& ctx) const
    {
        auto it = ctx.out();
        it      = std::format_to(it, "[x: {}, y: {}, z: {}]", v.x, v.y, v.z);
        return it;
    }
};
