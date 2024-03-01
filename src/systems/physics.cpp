#include <components/transform.hpp>
#include <components/velocity.hpp>
#include <systems/physics.hpp>
#include <fmt/format.h>

void wf::systems::physics::initialize(const wf::clock& clock)
{
    clock_ = clock;
}

void wf::systems::physics::update(entt::registry& entities)
{
    entities.view<components::velocity, components::transform>().each(
        [this](const components::velocity& velocity,
               components::transform& transform) {
            auto dt =
                std::chrono::duration<float>(clock_->get().delta()).count();
            transform.position += velocity.traversal * dt;
        });
}
