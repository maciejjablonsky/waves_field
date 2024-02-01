#include <boost/circular_buffer.hpp>
#include <components/grid.hpp>
#include <components/mesh.hpp>
#include <components/render.hpp>
#include <components/transform.hpp>
#include <config.hpp>
#include <fmt/chrono.h>
#include <glfw_glad.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <numeric>
#include <random>
#include <ranges>
#include <resource/shaders_manager.hpp>
#include <systems/renderer.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

extern "C"
{
    __declspec(dllexport) int NvOptimusEnablement = 0x00000001;
}

namespace wf::systems
{
void renderer::init_ft_()
{
    FT_Library ft;
    if (FT_Init_FreeType(std::addressof(ft)))
    {
        throw std::runtime_error(
            "ERROR::FREETYPE: Could not init FreeType Library");
    }

    FT_Face face;
    auto font_path = std::filesystem::path{RESOURCE_DIRECTORY} / "arial.ttf";

    if (FT_New_Face(ft, font_path.string().c_str(), 0, &face))
    {
        throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
    }
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            throw std::runtime_error("ERROR::FREETYTPE: Failed to load Glyph");
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x};
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenVertexArrays(1, &text_vao_);
    glGenBuffers(1, &text_vbo_);
    glBindVertexArray(text_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void display_control_cube(resource::shaders_manager& shaders_manager,
                          const glm::mat4& view_matrix,
                          const glm::mat4& projection_matrix)
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

    auto& shader = shaders_manager.get("control_cube");
    shader.use();

    // Set model, view, projection matrices here
    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.25f, 0.5f, -0.25f));
    // model = glm::rotate(glm::radians(180), model);
    model = glm::scale(model, glm::vec3{0.5f, 1.f, 0.5f});

    shader.set("model", model);
    shader.set("view", view_matrix);
    shader.set("projection", projection_matrix);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderer::framebuffer_size_callback(GLFWwindow* window,
                                         int width,
                                         int height)
{
    glViewport(0, 0, width, height);
}

void renderer::render_unit_axes_(resource::shaders_manager& shaders_manager,
                                 const glm::mat4& view_matrix,
                                 const glm::mat4& projection_matrix) const
{
    std::array vertices = {0.f,
                           0.f,
                           0.f,
                           1.f,
                           0.f,
                           0.f,
                           0.f,
                           0.f,
                           0.f,
                           0.f,
                           1.f,
                           0.f,
                           0.f,
                           0.f,
                           0.f,
                           0.f,
                           0.f,
                           -1.f};

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    auto& shader = shaders_manager.get("unit_axis");
    shader.use();

    glm::mat4 model{1.f};

    shader.set("model", model);
    shader.set("view", view_matrix);
    shader.set("projection", projection_matrix);

    glBindVertexArray(VAO);
    auto colors = {glm::vec3{1.f, 0.f, 0.f},
                   glm::vec3{0.f, 1.f, 0.f},
                   glm::vec3{0.f, 0.f, 1.f}};
    for (const auto& [i, color] : std::views::enumerate(colors))
    {
        shader.set("axis_color", color);
        glDrawArrays(GL_LINES, 2 * i, 2);
    }
    glBindVertexArray(0);
}

void renderer::initialize(const renderer_config& c)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfw_window_ = glfwCreateWindow(
        c.width(), c.height(), c.name().data(), nullptr, nullptr);

    if (glfw_window_ == nullptr)
    {
        throw std::runtime_error{"Failed to create GLFW window\n"};
    }

    glfwMakeContextCurrent(glfw_window_);
    glfwSetFramebufferSizeCallback(glfw_window_,
                                   renderer::framebuffer_size_callback);
    glfwSetWindowUserPointer(glfw_window_, this);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error{"Failed to initialize GLAD\n"};
    }
    init_ft_();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(40.f / 256, 0.f / 256, 75.f / 256, 1.f);
    viewport_ = {c.width(), c.height()};
    glViewport(0, 0, viewport_.x, viewport_.y);
    glfwSwapInterval(0);
}

renderer::~renderer() noexcept
{
    if (glfw_window_)
    {
        glfwDestroyWindow(glfw_window_);
    }
    glfwTerminate();
}

void renderer::render_grids_(entt::registry& entities,
                             resource::shaders_manager& shaders_manager,
                             const glm::mat4& view_matrix,
                             const glm::mat4& projection_matrix) const
{
    auto view = entities.view<components::transform,
                              components::render,
                              components::grid>();
    for (auto entity : view)
    {
        auto& grid      = view.get<components::grid>(entity);
        auto& render    = view.get<components::render>(entity);
        auto& transform = view.get<components::transform>(entity);
        auto& shader    = render.shader.get();

        shader.use();
        shader.set("view", view_matrix);
        shader.set("projection", projection_matrix);

        auto t_origin          = transform.position;
        auto heights           = grid.heights();
        glm::vec3 top_right    = {1.f, 0.f, 0.f};
        glm::vec3 top_left     = {0.f, 1.f, 0.f};
        glm::vec3 bottom_right = {0.f, 0.f, 0.2f};
        glm::vec3 bottom_left  = {1.f, 1.f, 0.f};

        for (int x = 0; x < grid.x_tiles(); ++x)
        {
            for (int z = 0; z < grid.z_tiles(); ++z)
            {
                float h                     = heights[std::array{x, z}];
                glm::vec3 relative_position = {x * grid.tile_size * 0.5f + 0.5f,
                                               0.5f,
                                               -(z * grid.tile_size * 0.5f + 0.5f)};
                transform.set_position(t_origin + relative_position);
                transform.set_scale({grid.tile_size, h, grid.tile_size});

                shader.set("model", transform.get_model_matrix());
                auto top = glm::mix(top_left, top_right, x / grid.width);
                auto bottom =
                    glm::mix(bottom_left, bottom_right, x / grid.width);
                shader.set("cube_color", glm::mix(top, bottom, z / grid.depth));
                glBindVertexArray(render.vao);
                glDrawArrays(GL_TRIANGLES, 0, render.vertices_number);
                glBindVertexArray(0);
            }
        }

        transform.set_position(std::move(t_origin));
        transform.set_scale({1.f, 1.f, 1.f});
    }
}

const gsl::not_null<const GLFWwindow*> renderer::get_glfw_window() const
{
    return glfw_window_;
}
gsl::not_null<GLFWwindow*> renderer::get_mutable_glfw_window()
{
    return glfw_window_;
}

glm::vec2 renderer::get_viewport() const
{
    return viewport_;
}

void renderer::update(entt::registry& entities,
                      resource::shaders_manager& shaders_manager,
                      const glm::mat4& view_matrix,
                      const glm::mat4& projection_matrix,
                      std::chrono::microseconds delta) const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    perf_.add(delta);
    render_text(shaders_manager.get("text"),
                fmt::format("{:5.2} ms/frame", perf_.time().count() / 1e3),
                10,
                static_cast<int>(viewport_.y - 100),
                0.7,
                {1.f, 0.f, 0.f});
    render_text(shaders_manager.get("text"),
                fmt::format("{:5.1f} fps", perf_.fps()),
                10,
                static_cast<int>(viewport_.y- 50),
                0.7,
                {1.f, 0.f, 0.f});
     //display_control_cube(shaders_manager, view_matrix, projection_matrix);
    render_unit_axes_(shaders_manager, view_matrix, projection_matrix);
    render_grids_(entities, shaders_manager, view_matrix, projection_matrix);

    glfwSwapBuffers(glfw_window_);
}

void renderer::render_text(wf::resource::shader_program& s,
                           const std::string& text,
                           float x,
                           float y,
                           float scale,
                           glm::vec3 color) const
{
    // activate corresponding render state
    s.use();
    glm::mat4 projection = glm::ortho(0.0f, viewport_.x, 0.0f, viewport_.y);
    s.set("projection", projection);
    s.set("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_vao_);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        const Character& ch = Characters.at(*c);

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {{xpos, ypos + h, 0.0f, 0.0f},
                                {xpos, ypos, 0.0f, 1.0f},
                                {xpos + w, ypos, 1.0f, 1.0f},

                                {xpos, ypos + h, 0.0f, 0.0f},
                                {xpos + w, ypos, 1.0f, 1.0f},
                                {xpos + w, ypos + h, 1.0f, 0.0f}};
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += (ch.Advance >> 6) *
             scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace wf::systems
