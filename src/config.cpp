#include "config.hpp"
#include <stdexcept>
#include <array>

namespace wf
{
scoped_file::~scoped_file()
{
    if (fp_)
    {
        std::fclose(fp_);
    }
}
scoped_file::operator std::FILE*()
{
    return fp_;
}

scoped_file::operator bool()
{
    return fp_ != nullptr;
}

window_config::window_config(const rapidjson::Value& windowValue)
{
    width_  = windowValue["width"].GetInt();
    height_ = windowValue["height"].GetInt();
    name_   = windowValue["name"].GetString();
}

int window_config::width() const
{
    return width_;
}
int window_config::height() const
{
    return height_;
}
const std::string& window_config::name() const
{
    return name_;
}

shaders_config::shaders_config(const rapidjson::Value& shadersValue)
{
    source_directory_ = shadersValue["source_directory"].GetString();
}

std::filesystem::path shaders_config::source_directory() const
{
    return source_directory_;
}

const window_config& config::window() const
{
    return windowConfig_;
}

const shaders_config& config::shaders() const
{
    return shadersConfig_;
}

config::config()
    : doc_([] {
          scoped_file f("../config.json", "r");
          if (not f)
          {
              throw std::runtime_error{"Failed to open config.json\n"};
          }
          std::array<char, 65536> read_buffer{};
          rapidjson::FileReadStream is(
              f, read_buffer.data(), read_buffer.size());
          rapidjson::Document doc;
          doc.ParseStream(is);
          return doc;
      }()),
      windowConfig_(doc_["window"]), shadersConfig_(doc_["shaders"])
{
}
} // namespace wf