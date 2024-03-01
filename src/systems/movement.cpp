#include <fmt/format.h>
#include <systems/movement.hpp>

namespace wf::systems
{
void movement::update_main_camera_velocity_(
    entt::registry& entities,
    input_commands::change_player_velocity_direction& c)
{
    c.mark_as_executed();
    for (auto entity : entities.view<components::camera>())
    {
        auto& camera = entities.get<components::camera>(entity);
        if (not camera.is_main())
        {
            continue;
        }

        if (not entities.all_of<components::movement, components::velocity>(
                entity))
        {
            auto& movement =
                entities.emplace_or_replace<components::movement>(entity);
            movement.mark_as_infinite();
            auto& velocity =
                entities.emplace_or_replace<components::velocity>(entity);
            velocity.traversal =
                player_speed *
                glm::normalize(camera.get_orientation() * c.direction);
        }
        else
        {
            auto& velocity         = entities.get<components::velocity>(entity);
            auto magnitude         = glm::length(velocity.traversal);
            auto current_direction = glm::normalize(velocity.traversal);
            auto new_direction =
                current_direction +
                glm::normalize(camera.get_orientation() * c.direction);
            velocity.traversal = magnitude * glm::normalize(new_direction);
        }
    }
}

void movement::update_main_camera_velocity_(entt::registry& entities,
                                            input_commands::stop_player& c)
{
    c.mark_as_executed();
    for (auto entity : entities.view<components::camera>())
    {
        auto& camera = entities.get<components::camera>(entity);
        if (not camera.is_main())
        {
            continue;
        }
        auto * movement = entities.try_get<components::movement>(entity);
        if (movement)
        {
            movement->stop();
        }
    }
}

void movement::initialize(const clock& clock)
{
    clock_ = clock;
}
} // namespace wf::systems