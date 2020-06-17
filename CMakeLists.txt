cmake_minimum_required(VERSION 3.16)
project(waves_field)

set(CMAKE_CXX_STANDARD 20)


add_subdirectory(dependencies/GLFW)
include_directories(dependencies/GLFW/include)

include_directories(include)
add_executable(waves_field src/main.cpp src/Program.cpp include/Program.h src/Window.cpp include/Window.h)

target_link_libraries(waves_field glfw ${GLFW_LIBRARIES} GL)
