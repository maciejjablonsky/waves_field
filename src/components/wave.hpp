#pragma once
#include <numbers>

namespace wf::components
{
struct wave
{
    float amplitude;
    float frequency;
    float phase_0 = 0.f;
    float speed;
    glm::vec3 direction;
    float duration{0.f}; // accumulation of delta times

    void update(float dt)
    {
        duration += dt;
    }

    float height(glm::vec3 position)
    {
        float omega    = 2 * std::numbers::pi * frequency;
        float distance = glm::dot(position, direction);
        float phase =
            std::fmod(phase_0 + omega * duration + (omega * distance / speed),
                      2 * std::numbers::pi);
        return amplitude * std::pow(std::numbers::e, std::sin(phase) - 1);
        // return amplitude * std::sin(phase);
    }
};
} // namespace wf::components
