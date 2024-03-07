#include "app.hpp"
#include <algorithm>
#include <boost/circular_buffer.hpp>
#include <clock.hpp>
#include <components/grid.hpp>
#include <components/material.hpp>
#include <components/mesh.hpp>
#include <components/render.hpp>
#include <components/transform.hpp>
#include <components/wave.hpp>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <numeric>
#include <random>
#include <resource/shaders_manager.hpp>
#include <systems/camera.hpp>
#include <systems/movement.hpp>
#include <systems/physics.hpp>
#include <systems/waves.hpp>

namespace wf
{
void create_waves_entity(entt::registry& entities,
                         resource::shaders_manager& shaders_manager)
{
    auto entity     = entities.create();
    auto& transform = entities.emplace<components::transform>(entity);

    auto& grid     = entities.emplace<components::grid>(entity);
    grid.tile_size = 16.f;
    grid.set_resolution(100.f, 100.f);

    auto& material = entities.emplace<components::material>(
        entity, shaders_manager.get("phong_light"));
    auto& render = entities.emplace<components::render>(
        entity, shaders_manager.get("phong_light"));

    auto& mesh = entities.emplace<components::mesh>(
        entity, render, grid, [](float x, float y) { return 0.f; });
}

app::app()
{
    const config config;
    clock clock;
    entt::registry entities;

    systems::camera camera;
    camera.initialize(glm::vec3{100, 200.2506, 100.22834},
                      glm::vec3{15.f, 0.f, -15.f},
                      entities);

    systems::renderer renderer;
    renderer.initialize(config.renderer(), clock, entities);

    systems::input input;
    input.initialize(renderer.get_mutable_glfw_window());

    systems::movement movement;
    movement.initialize(clock);

    systems::physics physics;
    physics.initialize(clock);

    resource::shaders_manager shaders_manager(config.shaders());
    shaders_manager.init();

    systems::waves waves;
    waves.initialize(entities);

    create_waves_entity(entities, shaders_manager);

    clock.tick();
    while (input.is_window_open())
    {
        clock.tick();
        input.update(entities);
        clock.update(input.get_mutable_commands_to_be_executed());
        camera.update(input.get_mutable_commands_to_be_executed(), entities);
        movement.update(input.get_mutable_commands_to_be_executed(), entities);
        waves.update(entities, clock);
        physics.update(entities);
        renderer.update(input.get_mutable_commands_to_be_executed(),
                        entities,
                        shaders_manager);
    }
}
} // namespace wf
