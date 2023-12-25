#include "utils.hpp"
#include <fmt/std.h>

namespace wf
{
void log(const std::string& message, const std::source_location& loc)
{
    fmt::print("Debug:{}:{}:{}\n", loc.file_name(), loc.line(), message);
}

std::string load_text_from_file(const std::filesystem::path& path)
{
    std::ifstream file{path};
    file.exceptions(std::istream::failbit | std::ifstream::badbit);
    std::string text = {std::istreambuf_iterator<char>{file}, {}};
    return text;
}
} // namespace wf