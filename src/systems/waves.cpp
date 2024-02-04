#include <array>
#include <cmath>
#include <components/grid.hpp>
#include <components/mesh.hpp>
#include <components/transform.hpp>
#include <numbers>
#include <systems/waves.hpp>

namespace wf::systems
{
void waves::update(entt::registry& entities, const clock& clock)
{
    auto view =
        entities.view<components::grid, components::mesh, components::render>();
    const float wave_speed  = -4.0f; // Speed of wave propagation
    const float wave_length = 1.0f; // Wavelength of the waves
    const float wave_height = 0.1f;  // Maximum height of the waves
    const float frequency   = std::numbers::pi / wave_length;

    float dt =  clock.delta().count() / 1e6;

    for (auto entity : view)
    {
        auto& grid   = view.get<components::grid>(entity);
        auto& mesh   = view.get<components::mesh>(entity);
        auto& render = view.get<components::render>(entity);

        glm::vec3 stone_throw_point{0.f, 0.f, 0.f};
        mesh.update(render, grid, [=](float x, float z) {
            auto distance = glm::length(glm::vec3{x, 0.f, z} * grid.tile_size -
                                        stone_throw_point);
            float wave =
                wave_height * sin(frequency * distance +
                                  wave_speed * clock.current().count() / 1e6);
            float damping =
                std::exp(-0.1f * distance) * std::exp(-0.05f * dt) * 3;

            return wave * distance;
        });
    }
}
} // namespace wf::systems
