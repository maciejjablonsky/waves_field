#pragma once
#include <mdspan>
#include <vector>

namespace wf::components
{
struct grid
{
  private:
    std::vector<float> height_;

  public:
    float tile_size{1.f};
    float width;
    float depth;

    auto heights()
    {
        return std::mdspan{height_.data(), x_tiles(), z_tiles()};
    }
    int x_tiles() const;
    int z_tiles() const;
    float height(int x, int z);
    void set_resolution(float width, float depth);
};
} // namespace wf::components