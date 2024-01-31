#include "grid.hpp"
#include <cmath>
#include <ranges>

namespace wf::components
{
int grid::x_tiles() const
{
    return std::floor(width / tile_size);
}

int grid::z_tiles() const
{

    return std::floor(depth / tile_size);
}

float grid::height(int x, int z)
{
    return height_.at(z * x_tiles() + x);
}

void grid::set_resolution(float width, float depth)
{
    this->width = width;
    this->depth = depth;
    height_.resize(x_tiles() * z_tiles());
    std::fill(std::begin(height_), std::end(height_), 1.f);
   }
}