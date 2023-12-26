#include "app.hpp"
#include "camera.hpp"
#include "grid.hpp"

namespace wf
{
void app::process_input_()
{
    if (glfwGetKey(glfw_window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(glfw_window_, true);
    }
    if (glfwGetKey(glfw_window_, GLFW_KEY_W) == GLFW_PRESS)
        camera_.process_keyboard(camera_movement::forward, clock_.delta());
    if (glfwGetKey(glfw_window_, GLFW_KEY_S) == GLFW_PRESS)
        camera_.process_keyboard(camera_movement::backward, clock_.delta());
    if (glfwGetKey(glfw_window_, GLFW_KEY_A) == GLFW_PRESS)
        camera_.process_keyboard(camera_movement::left, clock_.delta());
    if (glfwGetKey(glfw_window_, GLFW_KEY_D) == GLFW_PRESS)
        camera_.process_keyboard(camera_movement::right, clock_.delta());
    if (glfwGetKey(glfw_window_, GLFW_KEY_UP) == GLFW_PRESS)
        camera_.process_keyboard(camera_movement::up, clock_.delta());
    if (glfwGetKey(glfw_window_, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera_.process_keyboard(camera_movement::down, clock_.delta());
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback_fn(GLFWwindow* window, double xpos, double ypos)
{
    auto self = static_cast<app*>(glfwGetWindowUserPointer(window));
    if (self)
    {
        self->mouse_callback(xpos, ypos);
    }
}

void scroll_callback_fn(GLFWwindow* window, double xoffset, double yoffset)
{
    auto self = static_cast<app*>(glfwGetWindowUserPointer(window));
    if (self)
    {
        self->scroll_callback(xoffset, yoffset);
    }
}

app::app(const config& c)
    : camera_{c,
              glm::vec3{-115.509895, 116.12917, 239.37762},
              glm::vec3{0.f, 1.f, 0.f},
              -64.49996f,
              -26.10008f}
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
    glfwSetFramebufferSizeCallback(glfw_window_, framebuffer_size_callback);
    glfwSetCursorPosCallback(glfw_window_, mouse_callback_fn);
    glfwSetScrollCallback(glfw_window_, scroll_callback_fn);
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(glfw_window_, this);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error{"Failed to initialize GLAD\n"};
    }

    glViewport(0, 0, c.window().width(), c.window().height());

    wf::grid grid{c, camera_};

    clock_.tick();
    while (not glfwWindowShouldClose(glfw_window_))
    {
        clock_.tick();
        process_input_();
        //camera_.log();

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

void app::mouse_callback(double xpos_, double ypos_)
{
    float xpos = static_cast<float>(xpos_);
    float ypos = static_cast<float>(ypos_);

    if (first_frame_)
    {
        last_x_      = xpos;
        last_y_      = ypos;
        first_frame_ = false;
    }

    float xoffset = xpos - last_x_;
    float yoffset =
        last_y_ - ypos; // reversed since y-coordinates go from bottom to top

    last_x_ = xpos;
    last_y_ = ypos;

    camera_.process_mouse_movement(xoffset, yoffset);
}

void app::scroll_callback(double xoffset, double yoffset)
{
    camera_.process_mouse_scroll(yoffset);
}

float clock::delta() const
{
    return current_ - previous_;
}

void clock::tick()
{
    previous_ = current_;
    current_  = static_cast<float>(glfwGetTime());
}
} // namespace wf
