module;
#include <entt/entt.hpp>
#include <gsl/gsl>
#include <vulkan/vulkan.h>
export module systems.render;
import system;

namespace wf::systems
{
export class render2d
{
  public:
    void update(const entt::registry& ecs);
};
static_assert(wf::systems::system<render2d>);

export class render3d
{
  public:
    void update(const entt::registry& ecs);
};
static_assert(wf::systems::system<render3d>);
} // namespace wf::systems
