#include "render.hpp"

namespace wf::components
{
render::render(resource::shader_program& program) : shader{program}
{
    glGenVertexArrays(1, std::addressof(vao));
}

render::~render()
{
    glDeleteVertexArrays(1, std::addressof(vao));
}
} // namespace wf::components