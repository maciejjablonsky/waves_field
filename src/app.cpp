#include "app.hpp"
#include <fmt/format.h>
#include <resource/shaders_manager.hpp>
#include <systems/camera.hpp>

namespace wf
{

app::app()
{
    const config config;
    systems::renderer renderer;
    renderer.initialize(config.renderer());
    systems::input input;
    input.initialize(renderer.get_mutable_glfw_window());

    resource::shaders_manager shaders_manager(config.shaders());
    shaders_manager.init();

    entt::registry entities;
    systems::camera camera;
    camera.initialize(glm::vec3{5.f, 2.f, -5.f}, glm::vec3{0.f, 0.f, 0.f}, renderer.get_viewport());
    while (true)
    {
        input.update(entities);
        if (not input.is_window_open())
            break;
        camera.update(input.get_mutable_commands_to_be_executed());
        renderer.update(entities,
                        shaders_manager,
                        std::as_const(camera).get_view(),
                        std::as_const(camera).get_projection());
    }
}
} // namespace wf
