#pragma once
#include <components/transform.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utils.hpp>


namespace wf::components
{
class camera
{
  private:
    glm::quat orientation_;
    float zoom_{45.f};
    bool is_main_ = false;

  public:
    void update_orientation(const glm::quat& orientation);
    const glm::quat& get_orientation() const;
    void set_as_main();
    void unset_as_main();
    bool is_main() const;
    void update_zoom(float zoom);
    float get_zoom() const;
};
} // namespace wf::components

namespace wf
{
std::tuple<std::reference_wrapper<components::camera>,
           std::reference_wrapper<components::transform>>
find_main_camera(entt::registry& entities);
}
