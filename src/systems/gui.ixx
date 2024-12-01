module;
#include <boost/container/detail/advanced_insert_int.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <string>

export module systems.gui;
import system;
import components.transform;

namespace wf::gui
{
struct section
{
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 background;
};

struct text : section
{
    std::string content;
};
} // namespace wf::gui

namespace wf::systems
{
export class gui
{
  private:
    std::reference_wrapper<entt::registry> ecs_;
    entt::entity settings_entity_ = entt::null;

  public:
    gui(entt::registry& ecs, entt::entity settings_entity);
    void update();

    void add_section(const wf::gui::section& create_info);
};

gui::gui(entt::registry& ecs, entt::entity settings_entity)
    : ecs_{ecs}, settings_entity_{settings_entity}
{
}

void gui::update()
{
}

void gui::add_section(const wf::gui::section& create_info)
{
    auto& ecs = ecs_.get();
    auto e    = ecs.create();
    ecs.emplace<wf::components::transform2d>(e, create_info.position);
}
} // namespace wf::systems
