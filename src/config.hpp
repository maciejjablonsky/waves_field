#pragma once

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <string>
#include <filesystem>

namespace wf
{
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

class window_config
{
    int width_;
    int height_;
    std::string name_;

  public:
    window_config(const rapidjson::Value& windowValue);

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
    window_config windowConfig_;
    shaders_config shadersConfig_;

  public:
    config();

    const window_config& window() const;
    const shaders_config& shaders() const;
};

} // namespace wf