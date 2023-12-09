cmake_minimum_required(VERSION 3.15)
project(waves_field CXX)

find_package(glfw3 REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)
find_package(glad REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)

add_executable(waves_field src/main.cpp "src/app.ixx" )
target_link_libraries(waves_field PRIVATE fmt::fmt glfw glad::glad glm::glm)

install(TARGETS waves_field DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
