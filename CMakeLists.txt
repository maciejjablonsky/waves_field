cmake_minimum_required(VERSION 3.16)
project(waves_field)

set(CMAKE_CXX_STANDARD 20)


add_subdirectory(dependencies/GLFW)
include_directories(dependencies/GLFW/include)
add_executable(waves_field main.cpp Program.cpp Program.h Window.cpp Window.h)

target_link_libraries(waves_field glfw ${GLFW_LIBRARIES} GL)
