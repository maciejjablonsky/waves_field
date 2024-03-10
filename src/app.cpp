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

namespace wf
{
struct position_and_normal
{
    glm::vec3 position;
    glm::vec3 normal;
};

template <> struct vertex_attribute_pointer<position_and_normal>
{
    static void set_vertex_attribute_pointer(uint32_t& vertex_attribute_index)
    {
        using attributes_type = position_and_normal;

        static_assert(std::is_standard_layout_v<attributes_type>);
        static_assert(
            std::same_as<decltype(attributes_type::position)::value_type,
                         float>,
            "position vertex attrib pointer defined as float");
        glEnableVertexAttribArray(vertex_attribute_index);
        glVertexAttribPointer(vertex_attribute_index,
                              decltype(attributes_type::position)::length(),
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(position_and_normal),
                              (void*)offsetof(position_and_normal, position));

        static_assert(
            std::same_as<decltype(attributes_type::normal)::value_type, float>,
            "normal vertex attrib pointer defined as float");
        glEnableVertexAttribArray(vertex_attribute_index + 1);
        glVertexAttribPointer(vertex_attribute_index + 1,
                              decltype(attributes_type::normal)::length(),
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(position_and_normal),
                              (void*)offsetof(position_and_normal, normal));

        vertex_attribute_index += 2;
    }
};

std::vector<position_and_normal> create_net_positions_normals(float tile_size,
                                                              float width,
                                                              float depth)
{
    std::vector<position_and_normal> vertices;

    auto half_tile  = tile_size / 2;
    auto half_width = width / 2;
    auto half_depth = depth / 2;

    auto append_vertex = [&](glm::vec3 v) {
        vertices.emplace_back(position_and_normal{v, systems::y_unit});
    };

    for (float x = -half_width; x < half_width; x += tile_size)
    {
        for (float z = -half_depth; z < half_depth; z += tile_size)
        {
            append_vertex({x, 0.f, z + tile_size});
            append_vertex({x + tile_size, 0.f, z});
            append_vertex({x, 0.f, z});
            append_vertex({x, 0.f, z + tile_size});
            append_vertex({x + tile_size, 0.f, z + tile_size});
            append_vertex({x + tile_size, 0.f, z});
        }
    }
    return vertices;
}

components::mesh create_net_mesh(float tile_size, float width, float depth)
{
    components::mesh mesh{};
    auto positions_normals =
        create_net_positions_normals(tile_size, width, depth);
    mesh.assign(std::begin(positions_normals), std::end(positions_normals));

    return mesh;
}

void create_waves_entity(entt::registry& entities,
                         resource::shaders_manager& shaders_manager)
{
    auto entity     = entities.create();
    auto& transform = entities.emplace<components::transform>(entity);
    auto& material  = entities.emplace<components::material>(
        entity, shaders_manager.get("phong_light"));
    auto& mesh = entities.emplace<components::mesh>(
        entity, create_net_mesh(16.f, 300.f, 300.f));
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

    create_waves_entity(entities, shaders_manager);

    clock.tick();
    while (input.is_window_open())
    {
        clock.tick();
        input.update(entities);
        clock.update(input.get_mutable_commands_to_be_executed());
        camera.update(input.get_mutable_commands_to_be_executed(), entities);
        movement.update(input.get_mutable_commands_to_be_executed(), entities);
        physics.update(entities);
        renderer.update(input.get_mutable_commands_to_be_executed(),
                        entities,
                        shaders_manager);
    }
}
} // namespace wf
