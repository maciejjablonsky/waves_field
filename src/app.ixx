#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <string>

export module app;

namespace wf
{
export struct config
{
    int window_width        = 800;
    int window_height       = 600;
    std::string window_name = "waves_field";
};

export class app
{
  private:
    GLFWwindow* glfw_window_;
    void process_input_();

  public:
    app(const config& c = {});
    ~app() noexcept;
};

void app::process_input_()
{
    if (glfwGetKey(glfw_window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(glfw_window_, true);
    }
}

app::app(const config& c)
{
    glfwInit();
    glfw_window_ = glfwCreateWindow(c.window_width,
                                    c.window_height,
                                    c.window_name.c_str(),
                                    nullptr,
                                    nullptr);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (glfw_window_ == nullptr)
    {
        fmt::print("Failed to create GLFW window\n");
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(glfw_window_);
    // glfwSetFramebufferSizeCallback(glfw_window_, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    while (not glfwWindowShouldClose(glfw_window_))
    {
        process_input_();

        glfwSwapBuffers(glfw_window_);
        glfwPollEvents();
    }
}

app::~app() noexcept
{
    glfwDestroyWindow(glfw_window_);
    glfwTerminate();
}
} // namespace wf
