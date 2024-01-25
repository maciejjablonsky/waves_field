#pragma once

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <string>
#include <filesystem>

namespace wf
{
class renderer_config
{
    int width_;
    int height_;
    std::string name_;

  public:
    renderer_config(const rapidjson::Value& windowValue);

    int width() const;
    int height() const;
    const std::string& name() const;
};

class shaders_config
{
    std::string source_directory_;

  public:
    shaders_config(const rapidjson::Value& shadersValue);

    std::filesystem::path source_directory() const;
};

class config
{
    rapidjson::Document doc_;
    renderer_config renderer_config_;
    shaders_config shadersConfig_;

  public:
    config();

    const renderer_config& renderer() const;
    const shaders_config& shaders() const;

};

} // namespace wf