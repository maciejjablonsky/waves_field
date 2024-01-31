#include <array>
#include <cmath>
#include <components/grid.hpp>
#include <components/transform.hpp>
#include <numbers>
#include <systems/waves.hpp>

namespace wf::systems
{
void waves::update(entt::registry& entities, const clock& clock)
{
    auto view               = entities.view<components::grid>();
    const float wave_speed  = -4.0f;  // Speed of wave propagation
    const float wave_length = 10.0f;  // Wavelength of the waves
    const float wave_height = 10.0f; // Maximum height of the waves
    const float frequency   = std::numbers::pi / wave_length;

    float dt = clock.delta().count() / 1e6;

    for (auto entity : view)
    {
        auto& grid   = view.get<components::grid>(entity);
        auto heights = grid.heights();

        glm::vec3 stone_throw_point{grid.width / 2, 0.f, grid.depth / 2};

        for (int x = 0; x != grid.x_tiles(); ++x)
        {
            for (int z = 0; z != grid.z_tiles(); ++z)
            {
                auto distance = glm::length(
                    glm::vec3{x, 0.f, z} * grid.tile_size - stone_throw_point);

                float wave =
                    wave_height * cos(frequency * distance + wave_speed * clock.current().count() / 1e6);
                float damping =
                    std::exp(-0.1f * distance) * std::exp(-0.05f * dt) * 3;
                heights[std::array{x, z}] = wave * damping;
            }
        }
    }
}
} // namespace wf::systems
