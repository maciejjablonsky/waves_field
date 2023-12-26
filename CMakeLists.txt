cmake_minimum_required(VERSION 3.15)
project(waves_field CXX)

find_package(glfw3 REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)
find_package(glad REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(RapidJSON REQUIRED CONFIG)

message(STATUS "${CMAKE_VERSION} ")
set(CMAKE_CXX_MODULES ON)

set(SOURCES 
    src/app.cpp 
    src/app.hpp
    src/camera.cpp
    src/camera.hpp
    src/config.cpp 
    src/config.hpp
    src/grid.cpp 
    src/grid.hpp
    src/main.cpp 
    src/shader.cpp 
    src/shader.hpp
    src/utils.cpp 
    src/utils.hpp
)

add_executable(waves_field ${SOURCES})

target_link_libraries(waves_field PRIVATE fmt::fmt glfw glad::glad glm::glm rapidjson)

install(TARGETS waves_field DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )

set(SHADERS_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
configure_file(config.json.in config.json @ONLY)
add_custom_target(config_dependency DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config.json)
add_dependencies(waves_field config_dependency)
