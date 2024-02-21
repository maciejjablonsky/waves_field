#include <array>
#include <cmath>
#include <components/grid.hpp>
#include <components/mesh.hpp>
#include <components/transform.hpp>
#include <components/wave.hpp>
#include <numbers>
#include <random>
#include <systems/waves.hpp>

namespace wf::systems
{
void waves::initialize(entt::registry& entities)
{
    float amplitude     = 8.f;
    float frequency     = 0.1f;
    constexpr int count = 32;
    float speed         = 10.f;
    std::random_device dev;
    std::mt19937 rng(dev());
    for (int i = 0; i < count; ++i)
    {
        auto wave_entity = entities.create();
        auto direction   = [&] {
            std::uniform_real_distribution<float> dist(0, 2 * std::numbers::pi);
            float angle = dist(rng);
            return glm::normalize(
                glm::vec3{std::cos(angle), 0.f, std::sin(angle)});
        }();
        //direction = glm::normalize(glm::vec3{1, 0, 0} + glm::vec3{0, 0, -1});

        components::wave wave = {.amplitude = amplitude,
                                 .frequency = frequency,
                                 .speed     = speed,
                                 .direction = direction};
        entities.emplace<components::wave>(wave_entity, wave);

        amplitude *= 0.82;
        frequency *= 1.12;
    }
}
void waves::update(entt::registry& entities, const clock& clock)
{
    auto view =
        entities.view<components::grid, components::mesh, components::render>();

    float dt = clock.delta().count() / 1e6;

    for (auto entity : view)
    {
        auto& grid   = view.get<components::grid>(entity);
        auto& mesh   = view.get<components::mesh>(entity);
        auto& render = view.get<components::render>(entity);

        auto waves_view = entities.view<components::wave>();
        waves_view.each([=](auto& wave) { wave.update(dt); });

        mesh.update_continuous(render, grid, [&](float x, float z) {
            float wave_height = 0.f;
            waves_view.each([&](auto& wave) {
                wave_height += wave.height(glm::vec3(x, 0.f, z));
            });

            return wave_height;
        });
    }
}
} // namespace wf::systems
