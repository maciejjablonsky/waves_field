#include "config.hpp"
#include <stdexcept>
#include <array>
#include <utils.hpp>

namespace wf
{
renderer_config::renderer_config(const rapidjson::Value& windowValue)
{
    width_  = windowValue["width"].GetInt();
    height_ = windowValue["height"].GetInt();
    name_   = windowValue["name"].GetString();
}

int renderer_config::width() const
{
    return width_;
}
int renderer_config::height() const
{
    return height_;
}
const std::string& renderer_config::name() const
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

const renderer_config& config::renderer() const
{
    return renderer_config_;
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
      renderer_config_(doc_["renderer"]), shadersConfig_(doc_["shaders"])
{
}
} // namespace wf