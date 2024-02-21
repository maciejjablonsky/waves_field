#pragma once

#include <components/grid.hpp>
#include <components/render.hpp>
#include <filesystem>
#include <glm/glm.hpp>
#include <magic_enum/magic_enum.hpp>
#include <systems/unit_axes.hpp>
#include <utils.hpp>

namespace wf::components
{
struct mesh_vertex
{
    glm::vec3 position;
    glm::vec3 normal;
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

    void update_quantized(components::render& render_component,
                          const components::grid& grid_component,
                          wave_function auto height)
    {
        using systems::x_unit;
        using systems::y_unit;
        using systems::z_unit;
        vertices_.clear();

        auto tile       = grid_component.tile_size;
        auto half_tile  = tile / 2;
        auto half_width = grid_component.width / 2;
        auto half_depth = grid_component.depth / 2;
        vertices_.reserve(18 * ((grid_component.width / tile) + 1) *
                          ((grid_component.depth / tile) + 1));

        for (float x = -half_width; x < half_width; x += tile)
        {
            for (float z = -half_depth; z < half_depth; z += tile)
            {
                // up face
                auto up_y = height(x + half_tile, z + half_tile);
                vertices_.emplace_back<glm::vec3>({x, up_y, z}, y_unit);
                vertices_.emplace_back<glm::vec3>({x, up_y, z + tile}, y_unit);
                vertices_.emplace_back<glm::vec3>({x + tile, up_y, z + tile},
                                                  y_unit);

                vertices_.emplace_back<glm::vec3>({x + tile, up_y, z + tile},
                                                  y_unit);
                vertices_.emplace_back<glm::vec3>({x + tile, up_y, z}, y_unit);
                vertices_.emplace_back<glm::vec3>({x, up_y, z}, y_unit);

                // north face
                if (auto north_y = height(x + half_tile, z - tile + half_tile);
                    up_y >= north_y)
                {
                    vertices_.emplace_back<glm::vec3>({x, up_y, z}, -z_unit);
                    vertices_.emplace_back<glm::vec3>({x + tile, up_y, z},
                                                      -z_unit);
                    vertices_.emplace_back<glm::vec3>({x + tile, north_y, z},
                                                      -z_unit);

                    vertices_.emplace_back<glm::vec3>({x + tile, north_y, z},
                                                      -z_unit);
                    vertices_.emplace_back<glm::vec3>({x, north_y, z}, -z_unit);
                    vertices_.emplace_back<glm::vec3>({x, up_y, z}, -z_unit);
                }
                if (auto neighbour_south_y =
                        height(x + half_tile, z - tile + half_tile);
                    neighbour_south_y > up_y)
                {
                    vertices_.emplace_back<glm::vec3>({x, neighbour_south_y, z},
                                                      z_unit);
                    vertices_.emplace_back<glm::vec3>({x, up_y, z}, z_unit);
                    vertices_.emplace_back<glm::vec3>({x + tile, up_y, z},
                                                      z_unit);

                    vertices_.emplace_back<glm::vec3>({x + tile, up_y, z},
                                                      z_unit);
                    vertices_.emplace_back<glm::vec3>(
                        {x + tile, neighbour_south_y, z}, z_unit);
                    vertices_.emplace_back<glm::vec3>({x, neighbour_south_y, z},
                                                      z_unit);
                }

                if (auto west_y = height(x + half_tile - tile, z + half_tile);
                    up_y >= west_y)
                {
                    vertices_.emplace_back<glm::vec3>({x, up_y, z + tile},
                                                      -x_unit);
                    vertices_.emplace_back<glm::vec3>({x, up_y, z}, -x_unit);
                    vertices_.emplace_back<glm::vec3>({x, west_y, z}, -x_unit);

                    vertices_.emplace_back<glm::vec3>({x, west_y, z}, -x_unit);
                    vertices_.emplace_back<glm::vec3>({x, west_y, z + tile},
                                                      -x_unit);
                    vertices_.emplace_back<glm::vec3>({x, up_y, z + tile},
                                                      -x_unit);
                }
                if (auto neighbour_east_y =
                        height(x + half_tile - tile, z + half_tile);
                    neighbour_east_y > up_y)
                {

                    vertices_.emplace_back<glm::vec3>({x, up_y, z + tile},
                                                      x_unit);
                    vertices_.emplace_back<glm::vec3>({x, up_y, z}, x_unit);
                    vertices_.emplace_back<glm::vec3>({x, neighbour_east_y, z},
                                                      x_unit);

                    vertices_.emplace_back<glm::vec3>({x, neighbour_east_y, z},
                                                      x_unit);
                    vertices_.emplace_back<glm::vec3>(
                        {x, neighbour_east_y, z + tile}, x_unit);
                    vertices_.emplace_back<glm::vec3>({x, up_y, z + tile},
                                                      x_unit);
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

    void update_continuous(components::render& render_component,
                           const components::grid& grid_component,
                           wave_function auto height)
    {
        using systems::x_unit;
        using systems::y_unit;
        using systems::z_unit;
        vertices_.clear();

        auto tile       = grid_component.tile_size;
        auto half_tile  = tile / 2;
        auto half_width = grid_component.width / 2;
        auto half_depth = grid_component.depth / 2;

        auto compute_normal = [](glm::vec3 a, glm::vec3 b, glm::vec3 c) {
            auto ab = b - a;
            auto ac = c - a;
            return glm::normalize(glm::cross(ab, ac));
        };

        for (float x = -half_width; x < half_width; x += tile)
        {
            for (float z = -half_depth; z < half_depth; z += tile)
            {
                // first triangle
                glm::vec3 a   = {x, height(x, z + tile), z + tile};
                glm::vec3 b   = {x + tile, height(x + tile, z), z};
                glm::vec3 c   = {x, height(x, z), z};
                auto normal_1 = compute_normal(a, b, c);
                vertices_.emplace_back(a, normal_1);
                vertices_.emplace_back(b, normal_1);
                vertices_.emplace_back(c, normal_1);

                // second triangle
                glm::vec3 d = {x, height(x, z + tile), z + tile};
                glm::vec3 e = {x + tile, height(x + tile, z + tile), z + tile};
                glm::vec3 f = {x + tile, height(x + tile, z), z};
                auto normal_2 = compute_normal(d, e, f);
                vertices_.emplace_back(d, normal_2);
                vertices_.emplace_back(e, normal_2);
                vertices_.emplace_back(f, normal_2);
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
    }
};
} // namespace wf::components
