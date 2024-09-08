#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <cstdlib>
#include <entt/entt.hpp>
#include <filesystem>
#include <format>
#include <print>

import systems;
import systems.window;
import vk;

namespace wf
{
using namespace std::string_literals;
class app
{
  public:
    app()
    {
        entt::registry ecs;
        entt::entity settings = ecs.create();

        systems::window window_system("worlds simulator", settings);
        systems::input input_system(settings,
                                    window_system.get_window_handle());
        vk::instance vk_instance(window_system.get_window_handle());

        while (window_system.is_open())
        {
            window_system.update(ecs);
            input_system.update(ecs);
            vk_instance.draw_frame();
        }
        vk_instance.wait_device_idle();
    }
};
} // namespace wf

int main()
{
    try
    {
        if (const char* cwd = std::getenv("WAVES_FIELD_WORKING_DIR"))
        {
            std::println("Switching current working directory to {}", cwd);
            std::filesystem::current_path(cwd);
        }
        std::println("Current working directory: {}",
                     std::filesystem::current_path().string());

        wf::app app;
    }
    catch (const std::exception& e)
    {
        std::print("{}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
