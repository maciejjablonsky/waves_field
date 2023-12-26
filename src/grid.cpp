#include "grid.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace wf
{
std::vector<glm::vec3> grid::make_vertices_buffer_()
{
    std::vector<glm::vec3> vertices;
    vertices.reserve( 4 * (2 * grid_size + 1));

    for (int i = -grid_size; i <= grid_size; ++i)
    {
        // horizontal
        vertices.push_back({-grid_size * grid_spacing, 0.f, i * grid_spacing});
        vertices.push_back({grid_size * grid_spacing, 0.f, i * grid_spacing});

        // vertical
        vertices.push_back({i * grid_spacing, 0.f, -grid_size * grid_spacing});
        vertices.push_back({i * grid_spacing, 0.f, grid_size * grid_spacing});
    }

    return vertices;
}

grid::grid(const config& c, camera & camera)
    : shader_{c.shaders().source_directory() / "grid.vert",
              c.shaders().source_directory() / "grid.frag"}, 
    config_{c}, camera_{camera}

{
    glGenBuffers(1, std::addressof(vbo_));
    auto vertices   = make_vertices_buffer_();
    vertices_count_ = vertices.size();
    glGenVertexArrays(1, std::addressof(vao_));
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(glm::vec3),
                 vertices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void grid::display_control_cube_()
{
    float vertices[] = {
        // positions
        -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
        0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
        0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f};
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    shader cube_shader(config_.get().shaders().source_directory() /
                           "control_cube.vert",
        config_.get().shaders().source_directory() / "control_cube.frag");
    cube_shader.use();
    // Set model, view, projection matrices here
    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    cube_shader.set("model", model);
    cube_shader.set("view", camera_.get().view());
    cube_shader.set("projection", camera_.get().projection());

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void grid::show()
{
    glm::mat4 model =
        glm::mat4(1.0f); // Identity matrix - cube is at the origin

    shader_.use();
    shader_.set("model", model);
    shader_.set("view", camera_.get().view());
    shader_.set("projection", camera_.get().projection());

    glBindVertexArray(vao_); // Bind the VAO
    glDrawArrays(GL_LINES, 0, vertices_count_);
    glBindVertexArray(0); // Unbind the VAO
    display_control_cube_();
}

grid::~grid()
{
    glDeleteBuffers(1, std::addressof(vbo_));
}
} // namespace wf