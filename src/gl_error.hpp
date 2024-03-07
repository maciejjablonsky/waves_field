#pragma once

#include <fmt/format.h>
#include <glfw_glew.hpp>

#define ASSERT(x)                                                              \
    if (!(x))                                                                  \
        __debugbreak();

#define gl_call(x)                                                             \
    gl_clear_error();                                                          \
    x;                                                                         \
    ASSERT(gl_log_call(#x, __FILE__, __LINE__))

inline void gl_clear_error()
{
    while (glGetError() != GL_NO_ERROR)
    {
        ;
    }
}

inline bool gl_log_call(const char * function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        fmt::print("[OpenGL error][{}:{}:{}] ({})\n", file, function, line, error);
        return false;
    }
    return true;
}