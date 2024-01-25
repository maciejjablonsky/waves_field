#include <config.hpp>
#include <glfw_glad.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <resource/shaders_manager.hpp>
#include <systems/renderer.hpp>
#include <ranges>

namespace wf::systems
{
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
    std::ranges::for_each(vertices, [](float& v) { v += 0.5f; });
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
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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
		glDrawArrays(GL_LINES,2 * i, 2);
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

    glEnable(GL_DEPTH_TEST);
    viewport_ = {c.width(), c.height()};
    glViewport(0, 0, viewport_.x, viewport_.y);
}

renderer::~renderer() noexcept
{
    if (glfw_window_)
    {
        glfwDestroyWindow(glfw_window_);
    }
    glfwTerminate();
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

void renderer::update(const entt::registry& entities,
                      resource::shaders_manager& shaders_manager,
                      const glm::mat4& view_matrix,
                      const glm::mat4& projection_matrix) const
{
    glClearColor(40.f / 256, 0.f / 256, 75.f / 256, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    display_control_cube(shaders_manager, view_matrix, projection_matrix);
    render_unit_axes_(shaders_manager, view_matrix, projection_matrix);

    glfwSwapBuffers(glfw_window_);
}
} // namespace wf::systems