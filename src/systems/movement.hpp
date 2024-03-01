#pragma once
#include <clock.hpp>
#include <components/camera.hpp>
#include <components/movement.hpp>
#include <components/velocity.hpp>
#include <entt/entt.hpp>
#include <systems/input_command.hpp>

namespace wf
{
constexpr float player_speed = 50.f;
}

namespace wf::systems
{
class movement
{
  private:
    wf::optional_ref<const clock> clock_;

    void update_main_camera_velocity_(
        entt::registry& entities,
        input_commands::change_player_velocity_direction& c);
    void update_main_camera_velocity_(entt::registry& entities,
                                      input_commands::stop_player& c);

  public:
    void initialize(const clock& clock);
    void update(input_commands_view auto commands, entt::registry& entities);
};

void movement::update(input_commands_view auto commands,
                      entt::registry& entities)
{
    std::ranges::for_each(commands, [this, &entities](input_command& command) {
        std::visit(
            overloaded{
                [](const auto&) {},
                [this, &entities](
                    input_commands::change_player_velocity_direction& c) {
                    update_main_camera_velocity_(entities, c);
                },
                [this, &entities](input_commands::stop_player& c) {
                    update_main_camera_velocity_(entities, c);
                }},
            command);
    });

    entities.view<components::movement, components::velocity>().each(
        [&entities,
         this](auto entity, const components::movement& movement, ...) {
            if (not movement.is_happening(clock_->get().current()))
            {
                entities.erase<components::movement, components::velocity>(
                    entity);
            }
        });
}
} // namespace wf::systems