module;
#include <entt/entt.hpp>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <gsl/pointers>
#include <string>
#include <string_view>
export module systems.window;
import system;
import utils;
import commands;

namespace wf::systems
{
glm::i32vec2 get_screen_size()
{
    auto monitor    = glfwGetPrimaryMonitor();
    auto video_mode = glfwGetVideoMode(monitor);
    return {video_mode->width, video_mode->height};
}

std::tuple<glm::i32vec2, int> get_window_size()
{
    auto screen_size  = get_screen_size();
    auto smaller_size = std::min(screen_size.x, screen_size.y);
    auto border_size  = wf::to<int>(smaller_size * 0.15);
    return std::make_tuple(
        glm::i32vec2{screen_size.x - border_size, screen_size.y - border_size},
        border_size / 2);
}

export class window
{
  private:
    GLFWwindow* glfw_window_{};
    std::string window_title_{};
    entt::entity settings_entity_ = entt::null;
    bool window_open_             = true;

    void update_window_open_(entt::registry& ecs);

  public:
    window(std::string_view window_title, entt::entity settings_entity);
    ~window();

    gsl::not_null<GLFWwindow*> get_window_handle();
    void update(entt::registry& registry);
    [[nodiscard]] bool is_open() noexcept;
};

window::window(std::string_view window_title, entt::entity settings_entity)
    : window_title_(window_title), settings_entity_(settings_entity)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto [window_size, border_size] = get_window_size();
    glfw_window_                    = glfwCreateWindow(
        window_size.x, window_size.y, window_title_.c_str(), nullptr, nullptr);
    if (not glfw_window_)
    {
        throw std::invalid_argument("glfwCreateWindow failed");
    }
    glfwSetWindowPos(glfw_window_, border_size, border_size);
}

window::~window()
{
    glfwDestroyWindow(glfw_window_);
    glfwTerminate();
}

gsl::not_null<GLFWwindow*> window::get_window_handle()
{
    return glfw_window_;
}

void window::update(entt::registry& ecs)
{
    glfwPollEvents();
    update_window_open_(ecs);
}

bool window::is_open() noexcept
{
    return window_open_;
}

void window::update_window_open_(entt::registry& ecs)
{
    if (ecs.all_of<commands::exit>(settings_entity_))
    {
        window_open_ = false;
        ecs.erase<commands::exit>(settings_entity_);
    }
}
static_assert(wf::systems::system<window>);
} // namespace wf::systems
