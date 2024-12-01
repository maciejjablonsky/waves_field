module;
#include <cstdint>
export module systems.keys;

namespace wf::systems
{
export enum class key : uint16_t {
    escape,
    w,
    s,
    a,
    d,
};
export enum class key_state : uint8_t {
    pressed,
    released,
    idle,
    active,
};
} // namespace wf::systems