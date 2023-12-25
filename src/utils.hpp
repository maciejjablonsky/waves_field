#pragma once
#include <string>
#include <source_location>
#include <filesystem>

namespace wf
{
	
 void log(const std::string& message,
         const std::source_location& loc = std::source_location::current());

std::string load_text_from_file(const std::filesystem::path& path);
}