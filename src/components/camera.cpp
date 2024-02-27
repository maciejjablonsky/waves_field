#include <components/camera.hpp>

namespace wf::components
{
void camera::update_orientation(const glm::quat& orientation)
{
    orientation_ = orientation;
}
const glm::quat& camera::get_orientation() const
{
    return orientation_;
}

void wf::components::camera::set_as_main()
{
    is_main_ = true;
}

void camera::unset_as_main()
{
    is_main_ = false;
}

bool camera::is_main() const
{
    return is_main_;
}

void camera::update_zoom(float zoom)
{
    zoom_ = zoom;
}

float camera::get_zoom() const
{
    return zoom_;
}
} // namespace wf::components

namespace wf
{
std::tuple<std::reference_wrapper<components::camera>,
           std::reference_wrapper<components::transform>>
find_main_camera(entt::registry& entities)
{
    optional_ref<components::camera> main_camera = std::nullopt;
    optional_ref<components::transform> main_camera_transform_component =
        std::nullopt;
    entities.view<components::camera, components::transform>().each(
        [&main_camera, &main_camera_transform_component](
            auto entity, auto& camera_component, auto& transform_component) {
            if (camera_component.is_main())
            {
                main_camera                     = camera_component;
                main_camera_transform_component = transform_component;
            }
        });
    if (not main_camera)
    {
        throw std::runtime_error{"main camera doesn't exist"};
    }
    return {main_camera->get(), main_camera_transform_component->get()};
}
} // namespace wf
