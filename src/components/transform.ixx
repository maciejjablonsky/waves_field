module;
#include <glm/glm.hpp>
export module components.transform;

namespace wf::components
{
export struct transform2d
{
};

export struct transform3d
{
    glm::vec2 position;
    glm::vec2 scale;
};
} // namespace wf::components
