#pragma once

#include <components/grid.hpp>
#include <components/render.hpp>
#include <filesystem>
#include <glm/glm.hpp>
#include <magic_enum/magic_enum.hpp>
#include <utils.hpp>

namespace wf::components
{
struct mesh_vertex
{
    glm::vec3 position;
};

template <typename T>
concept wave_function = requires(T t, float x, float z) {
    {
        t(x, z)
    } -> std::convertible_to<float>;
};

class mesh : wf::non_copyable
{
  private:
    std::optional<uint32_t> vbo_;
    std::vector<mesh_vertex> vertices_;
    void cpu_to_gpu_vertex_buffer(GLuint vao,
                                  const std::vector<mesh_vertex>& cpu_data);

  public:
    mesh(components::render& render_component,
         const std::filesystem::path& mesh_path);

    void update(components::render& render_component,
                const components::grid& grid_component,
                wave_function auto height)
    {
        vertices_.clear();

        auto tile       = grid_component.tile_size;
        auto half_tile  = tile / 2;
        auto half_width = grid_component.width / 2;
        auto half_depth = grid_component.depth / 2;
        for (float x = -half_width; x < half_width; x += tile)
        {
            for (float z = -half_depth; z < half_depth; z += tile)
            {
                // up face
                auto up_y = height(x + half_tile, z + half_tile);
                vertices_.emplace_back<glm::vec3>({x, up_y, z});
                vertices_.emplace_back<glm::vec3>({x, up_y, z + tile});
                vertices_.emplace_back<glm::vec3>({x + tile, up_y, z + tile});

                vertices_.emplace_back<glm::vec3>({x + tile, up_y, z + tile});
                vertices_.emplace_back<glm::vec3>({x + tile, up_y, z});
                vertices_.emplace_back<glm::vec3>({x, up_y, z});

                // north face
                if (auto north_y = height(x + half_tile, z - tile + half_tile);
                    up_y >= north_y)
                {
                    vertices_.emplace_back<glm::vec3>({x, north_y, z});
                    vertices_.emplace_back<glm::vec3>({x + tile, north_y, z});
                    vertices_.emplace_back<glm::vec3>({x + tile, up_y, z});

                    vertices_.emplace_back<glm::vec3>({x + tile, up_y, z});
                    vertices_.emplace_back<glm::vec3>({x, up_y, z});
                    vertices_.emplace_back<glm::vec3>({x, north_y, z});
                }
                if (auto neighbour_south_y =
                        height(x + half_tile, z - tile + half_tile);
                    neighbour_south_y > up_y)
                {
                    vertices_.emplace_back<glm::vec3>(
                        {x, neighbour_south_y, z});
                    vertices_.emplace_back<glm::vec3>({x, up_y, z});
                    vertices_.emplace_back<glm::vec3>({x + tile, up_y, z});

                    vertices_.emplace_back<glm::vec3>({x + tile, up_y, z});
                    vertices_.emplace_back<glm::vec3>(
                        {x + tile, neighbour_south_y, z});
                    vertices_.emplace_back<glm::vec3>(
                        {x, neighbour_south_y, z});
                }
            }
        }
        if (vbo_.has_value())
        {
            assert(vertices_.size() == render_component.vertices_number);
        }
        cpu_to_gpu_vertex_buffer(render_component.vao, vertices_);
        render_component.vertices_number = vertices_.size();
    }

    mesh(
        components::render& render_component,
        const components::grid& grid_component,
        wave_function auto height = [](float x, float y) { return 0.f; })
    {
        update(render_component, grid_component, height);
    }
};
} // namespace wf::components