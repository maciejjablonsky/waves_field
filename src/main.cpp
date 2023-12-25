#include "app.hpp"
#include <fmt/std.h>

int main()
{
    try
    {
        wf::app app;
    }
    catch (const std::exception& e)
    {
        fmt::print("{}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
