#include <components/transform.hpp>

namespace wf::components
{
void transform::set_position(glm::vec3&& v)
{
    position = std::move(v);
    updated  = false;
}

void transform::set_rotation(glm::quat&& q)
{
    rotation = std::move(q);
    updated  = false;
}

void transform::set_scale(glm::vec3&& s)
{
    scale = std::move(s);
    updated = false;
}

const glm::mat4& transform::get_model_matrix()
{
    if (not updated)
    {
        model = glm::mat4(1.f); 
        model = glm::scale(model, scale);
        model = model * glm::mat4_cast(rotation);
        model = glm::translate(model, position);
        updated = true;
    }
    return model;
}
}