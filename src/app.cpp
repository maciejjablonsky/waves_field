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
    // glm::vec3 normal;
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

        // static_assert(
        //     std::same_as<decltype(attributes_type::normal)::value_type,
        //     float>, "normal vertex attrib pointer defined as float");
        // glEnableVertexAttribArray(vertex_attribute_index + 1);
        // glVertexAttribPointer(vertex_attribute_index + 1,
        //                       decltype(attributes_type::normal)::length(),
        //                       GL_FLOAT,
        //                       GL_FALSE,
        //                       sizeof(position_and_normal),
        //                       (void*)offsetof(position_and_normal, normal));

        vertex_attribute_index += 1;
    }
};

std::vector<position_and_normal> create_net_positions_normals(float tile_size,
                                                              float width,
                                                              float depth)
{
    int x = width / tile_size;
    int z = depth / tile_size;
    std::vector<position_and_normal> vertices;
    vertices.reserve(x * z * 6);

    auto half_tile  = tile_size / 2;
    auto half_width = width / 2;
    auto half_depth = depth / 2;

    auto append_vertex = [&](glm::vec3 v) {
        vertices.emplace_back(position_and_normal{v /*, systems::y_unit*/});
    };

    for (float x = -half_width; x < half_width; x += tile_size)
    {
        for (float z = -half_depth; z < half_depth; z += tile_size)
        {
            constexpr auto y = 0.f;
            append_vertex({x, y, z + tile_size});
            append_vertex({x + tile_size, y, z});
            append_vertex({x, y, z});
            append_vertex({x, y, z + tile_size});
            append_vertex({x + tile_size, y, z + tile_size});
            append_vertex({x + tile_size, y, z});
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

struct alignas(16) uniform_float
{
    float value;
};

struct alignas(16) uniform_vec3
{
    glm::vec3 value;
};

auto create_waves()
{
    std::vector<uniform_float> amplitudes;
    std::vector<uniform_float> frequencies;
    std::vector<uniform_float> speeds;
    std::vector<uniform_float> zero_phases;
    std::vector<uniform_vec3> directions;

    float amplitude     = 16.f;
    float frequency     = 0.03f;
    constexpr int count = 32;
    float speed         = 10.f;
    std::random_device dev;
    std::mt19937 rng(dev());
    for (int i = 0; i < count; ++i)
    {
        auto random_angle = [&] {
            std::uniform_real_distribution<float> dist(0, 2 * std::numbers::pi);
            return dist(rng);
        };

        auto direction = [&] {
            auto angle = random_angle();
            return glm::normalize(
                glm::vec3{std::cos(angle), 0.f, std::sin(angle)});
        }();

        amplitudes.push_back({amplitude});
        frequencies.push_back({frequency});
        speeds.push_back({speed});
        zero_phases.push_back({random_angle()});
        directions.push_back({direction});

        amplitude *= 0.82;
        frequency *= 1.12;
    }
    return std::make_tuple(
        amplitudes, frequencies, speeds, zero_phases, directions);
}

void create_waves_entity(entt::registry& entities,
                         resource::shaders_manager& shaders_manager)
{
    auto entity     = entities.create();
    auto& transform = entities.emplace<components::transform>(entity);
    auto& shader    = shaders_manager.get("phong_light");
    auto& material  = entities.emplace<components::material>(entity, shader);
    material.bind();

    material.assign_uniform("u_light_position", glm::vec3{0.f, 2000.f, 600.f});
    material.assign_uniform("u_light_color", glm::vec3{252, 229, 212} / 256.f);
    material.assign_uniform("u_object_color", glm::vec3{0.f, 0.19f, 0.39f});

    auto [amplitudes, frequencies, speeds, zero_phases, directions] =
        create_waves();
    material.assign_uniform(
        "u_amplitude[0]", std::begin(amplitudes), std::end(amplitudes));
    material.assign_uniform(
        "u_frequency[0]", std::begin(frequencies), std::end(frequencies));
    material.assign_uniform("u_speed[0]", std::begin(speeds), std::end(speeds));
    material.assign_uniform(
        "u_direction[0]", std::begin(directions), std::end(directions));

    auto& mesh = entities.emplace<components::mesh>(
        entity, create_net_mesh(1, 3000.f, 3000.f));
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
