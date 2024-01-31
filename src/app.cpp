#include "app.hpp"
#include <clock.hpp>
#include <components/grid.hpp>
#include <components/mesh.hpp>
#include <components/render.hpp>
#include <components/transform.hpp>
#include <fmt/format.h>
#include <resource/shaders_manager.hpp>
#include <systems/camera.hpp>
#include <systems/waves.hpp>

namespace wf
{
void create_waves_entity(entt::registry& entities,
                         resource::shaders_manager& shaders_manager)
{
    auto entity        = entities.create();
    auto& transform    = entities.emplace<components::transform>(entity);
    transform.position = {0.f, 0.f, 0.f};

    auto& grid = entities.emplace<components::grid>(entity);
    grid.tile_size = 1;
    grid.set_resolution(100.f, 100.f);

    auto& render = entities.emplace<components::render>(
        entity, shaders_manager.get("control_cube"));

    auto& mesh = entities.emplace<components::mesh>(
        entity,render, std::filesystem::path{RESOURCE_DIRECTORY} / "cube.obj");
}

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
    camera.initialize(glm::vec3{-4.0375, 34.2506, 10.22834},
                      glm::vec3{15.f, 0.f, -15.f},
                      renderer.get_viewport());
    systems::waves waves;

    clock clock;

    create_waves_entity(entities, shaders_manager);

    clock.tick();
    while (true)
    {
        clock.tick();
        input.update(entities);
        if (not input.is_window_open())
            break;
        waves.update(entities, clock);
        camera.update(input.get_mutable_commands_to_be_executed());
        renderer.update(entities,
                        shaders_manager,
                        std::as_const(camera).get_view(),
                        std::as_const(camera).get_projection());
    }
}
} // namespace wf
