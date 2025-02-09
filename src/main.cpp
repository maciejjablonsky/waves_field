#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <cstdlib>
#include <entt/entt.hpp>
#include <filesystem>
#include <format>
#include <future>
#include <print>
#include <thread>

import wf.ui;
import wf.vk;

int main(int argc, char** argv)
{
    try
    {
        if (auto cwd = std::getenv("WAVES_FIELD_WORKING_DIR"))
        {
            std::filesystem::current_path(cwd);
        }
        std::promise<wf::window_handle> window_handle{};
        std::jthread gui_thread([=, &window_handle] {
            wf::ui ui(argc, argv);
            window_handle.set_value(ui.handle());
            return ui.run();
        });

        std::jthread engine_thread([&window_handle] {
            auto handle = window_handle.get_future().get();
            wf::vk::instance vk{handle.id};
            while (1)
            {
            vk.draw_frame();
            }
            // wf::vk::instance
        });

    }
    catch (const std::exception& e)
    {
        std::print("{}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
