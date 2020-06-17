#include "../include/Window.h"
#include <iostream>

Window::Window() {
        if (!glfwInit()) {
                std::cout << "Failed to initialize glfw.\n";
                exit(-1);
        }
}

Window::~Window() {
        glfwTerminate();
}
