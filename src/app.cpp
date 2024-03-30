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
struct position
{
    glm::vec3 position;
    // glm::vec3 normal;
};

template <> struct vertex_attribute_pointer<position>
{
    static void set_vertex_attribute_pointer(uint32_t& vertex_attribute_index)
    {
        using attributes_type = position;

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
                              sizeof(position),
                              (void*)offsetof(position, position));

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

constexpr bool check_if_multiple(float length, float tile_size)
{
    assert(length > 0 && "trying divide non positive length");
    assert(tile_size > 0 && "tile can't be of non positive size");
    auto how_many_fit = length / tile_size;
    return (how_many_fit - static_cast<int>(how_many_fit)) < 1e-4;
}

std::vector<position> create_net_positions(float tile_size,
                                           float width,
                                           float depth)
{
    assert(check_if_multiple(width, tile_size) &&
           "width has to be multiple of tile size");
    assert(check_if_multiple(depth, tile_size) &&
           "depth has to be multiple of tile size");

    const float min_x = -(width / 2);
    const float max_x = (width / 2);
    const float min_z = -(depth / 2);
    const float max_z = (depth / 2);

    std::vector<position> vertices;
    auto lower_eq = [](float a, float b) {
        if (a < b)
        {
            return true;
        }
        return std::abs(a - b) < 1e-4;
    };

    for (float z = min_z; lower_eq(z, max_z); z += tile_size)
    {
        for (float x = min_x; lower_eq(x, max_x); x += tile_size)
        {
            vertices.emplace_back<glm::vec3>({x, 0.f, z});
        }
    }

    return vertices;
}

std::vector<uint32_t> create_net_indices(float tile_size,
                                         float width,
                                         float depth)
{
    assert(check_if_multiple(width, tile_size) &&
           "width has to be multiple of tile size");
    assert(check_if_multiple(depth, tile_size) &&
           "depth has to be multiple of tile size");

    int x_vertices_count = (width / tile_size) + 1;
    int z_vertices_count = (depth / tile_size) + 1;
    std::vector<uint32_t> indices;
    for (int z = 0; z < z_vertices_count - 1; ++z)
    {
        for (int x = 0; x < x_vertices_count - 1; ++x)
        {
            // width = 4, depth = 4, tile_size = 1
            //  0  1  2  3
            //  4  5  6  7
            //  8  9 10 11
            // 12 13 14 15
            int topLeft     = z * x_vertices_count + x;
            int topRight    = topLeft + 1;
            int bottomLeft  = (z + 1) * x_vertices_count + x;
            int bottomRight = bottomLeft + 1;
            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            // indices.push_back(topLeft);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
            // indices.push_back(topRight);
        }
    }
    return indices;
}

components::mesh create_net_mesh(float tile_size, float width, float depth)
{
    components::mesh mesh{};
    auto positions = create_net_positions(tile_size, width, depth);
    mesh.assign(std::begin(positions), std::end(positions));

    auto indices = create_net_indices(tile_size, width, depth);
    mesh.assign_indices(std::begin(indices), std::end(indices));

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

auto create_waves(float tile_size)
{
    std::vector<uniform_float> amplitudes;
    std::vector<uniform_float> frequencies;
    std::vector<uniform_float> speeds;
    std::vector<uniform_float> zero_phases;
    std::vector<uniform_vec3> directions;

    float amplitude     = 10.f;
    float wavelength    = 1000.f;
    constexpr int count = 32;
    float speed         = 20.f;
    float frequency     = speed / wavelength;
    fmt::print("initial wave frequency: {} Hz\n", frequency);
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

        assert((speed / wavelength) <= 2 * (speed / tile_size) &&
               "nyquist law invalidated");

        amplitudes.push_back({amplitude});
        frequencies.push_back({frequency});
        speeds.push_back({speed});
        zero_phases.push_back({random_angle()});
        directions.push_back({direction});

        amplitude *= 0.78;
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

    constexpr float tile_size = 1.f;
    auto [amplitudes, frequencies, speeds, zero_phases, directions] =
        create_waves(tile_size);
    material.assign_uniform(
        "u_amplitude[0]", std::begin(amplitudes), std::end(amplitudes));
    material.assign_uniform(
        "u_frequency[0]", std::begin(frequencies), std::end(frequencies));
    material.assign_uniform("u_speed[0]", std::begin(speeds), std::end(speeds));
    material.assign_uniform(
        "u_direction[0]", std::begin(directions), std::end(directions));

    auto& mesh = entities.emplace<components::mesh>(
        entity, create_net_mesh(tile_size, 2000.f, 2000.f));
}

app::app()
{
    const config config;
    clock clock;
    entt::registry entities;

    systems::camera camera;
    camera.initialize(glm::vec3{100, 1000.2506, 100.22834},
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
