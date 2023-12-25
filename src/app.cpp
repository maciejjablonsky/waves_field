#include "app.hpp"
#include "grid.hpp"

namespace wf
{
void app::process_input_()
{
    if (glfwGetKey(glfw_window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(glfw_window_, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

app::app(const config& c)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfw_window_ = glfwCreateWindow(c.window().width(),
                                    c.window().height(),
                                    c.window().name().data(),
                                    nullptr,
                                    nullptr);

    if (glfw_window_ == nullptr)
    {
        throw std::runtime_error{"Failed to create GLFW window\n"};
    }

    glfwMakeContextCurrent(glfw_window_);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error{"Failed to initialize GLAD\n"};
    }

    glViewport(0, 0, c.window().width(), c.window().height());
    glfwSetFramebufferSizeCallback(glfw_window_, framebuffer_size_callback);

    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);

    wf::grid grid{c};

    while (not glfwWindowShouldClose(glfw_window_))
    {
        process_input_();

        glClearColor(40.f / 256, 0.f / 256, 75.f / 256, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        grid.show();

        glfwSwapBuffers(glfw_window_);
        glfwPollEvents();
    }
}

app::~app() noexcept
{
    if (glfw_window_)
    {
        glfwDestroyWindow(glfw_window_);
    }
    glfwTerminate();
}
}
