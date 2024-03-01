#pragma once

#include <clock.hpp>
#include <entt/entt.hpp>
#include <utils.hpp>
#include <systems/input_command.hpp>

namespace wf::systems
{
class physics
{
  private:
    wf::optional_ref<const wf::clock> clock_{};

  public:
    void initialize(const wf::clock& clock);
    void update(entt::registry& entities);
};
} // namespace wf::systems
