#pragma once
#include <entt/entt.hpp>
#include <chrono>
#include <clock.hpp>

namespace wf::systems
{
class waves
{
  public:
    void update(entt::registry& entities, const clock & clock);

};
}