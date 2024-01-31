#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace wf::components
{
struct transform
{
    glm::vec3 position = {0.f, 0.f, 0.f};
    glm::quat rotation = {1.f, 0.f, 0.f, 0.f};
    glm::vec3 scale    = {1.f, 1.f, 1.f};

    glm::mat4 model;
    bool updated = false;
    
    void set_position(glm::vec3&& v);
    void set_rotation(glm::quat&& q);
    void set_scale(glm::vec3&& s);
    const glm::mat4 &get_model_matrix() ;
};
}