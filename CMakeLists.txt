cmake_minimum_required(VERSION 3.15)
project(waves_field CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)
# find_package(glad REQUIRED CONFIG)
find_package(glew REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(RapidJSON REQUIRED CONFIG)
find_package(EnTT REQUIRED CONFIG)
find_package(tinyobjloader REQUIRED CONFIG)
find_package(Microsoft.GSL REQUIRED CONFIG)
find_package(magic_enum REQUIRED CONFIG)
find_package(boost REQUIRED CONFIG)
find_package(freetype REQUIRED CONFIG)
find_package(range-v3 REQUIRED CONFIG)

message(STATUS "${CMAKE_VERSION} ")
set(CMAKE_CXX_MODULES ON)

add_executable(waves_field)
target_sources(waves_field PUBLIC
	src/systems/input.cpp
    src/app.cpp
    src/clock.cpp
    src/components/grid.cpp
    src/components/mesh.cpp
    src/components/render.cpp
    src/components/transform.cpp
    src/config.cpp
    src/main.cpp
    src/resource/shaders_manager.cpp
    src/systems/camera.cpp
    src/systems/pc_input.cpp
    src/systems/physics.cpp
    src/systems/renderer.cpp
    src/systems/waves.cpp
    src/components/camera.cpp
    src/utils.cpp
    src/components/movement.cpp
    src/systems/movement.cpp
    src/components/material.cpp
    src/uniform_buffer.cpp

    PUBLIC
    FILE_SET HEADERS 
    BASE_DIRS src
    FILES
        src/components/camera.hpp
		src/components/mesh.hpp
		src/systems/camera.hpp
		src/systems/input.hpp
        src/app.hpp
        src/clock.hpp
        src/components/grid.hpp
        src/components/grid.hpp
        src/components/render.hpp
        src/components/transform.hpp
        src/config.hpp
        src/glfw_glew.hpp
        src/resource/shaders_manager.hpp
        src/systems/input_command.hpp
        src/systems/pc_input.hpp
        src/systems/physics.hpp
        src/systems/renderer.hpp
        src/systems/unit_axes.hpp
        src/systems/waves.hpp
        src/utils.hpp
        src/components/velocity.hpp
        src/components/movement.hpp
        src/systems/movement.hpp
        src/components/material.hpp
        src/vertex_buffer_layout.hpp
        src/uniform_buffer.hpp
        src/gl_error.hpp
)

target_link_libraries(waves_field PRIVATE fmt::fmt glfw glm::glm rapidjson EnTT::EnTT tinyobjloader::tinyobjloader Microsoft.GSL::GSL magic_enum::magic_enum Boost::headers Freetype::Freetype range-v3::range-v3 GLEW::GLEW)


install(TARGETS waves_field DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )

set(SHADERS_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
configure_file(config.json.in config.json @ONLY)
add_custom_target(config_dependency DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config.json)
add_dependencies(waves_field config_dependency)

set(RESOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/resource)
configure_file(resource/waves_scene.json.in waves_scene.json @ONLY)
add_custom_target(waves_scene_dependency DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/waves_scene.json)
add_dependencies(waves_field waves_scene_dependency)

target_compile_definitions(waves_field PRIVATE "RESOURCE_DIRECTORY=\"${RESOURCE_DIRECTORY}\"")

if (MSVC)
    target_compile_options(waves_field PRIVATE
        $<$<CONFIG:Release>:/O2> # Maximize speed
        $<$<CONFIG:Release>:/Oi> # Enable intrinsic functions
        $<$<CONFIG:Release>:/Ot> # Favor fast code
        $<$<CONFIG:Release>:/Oy> # Omit frame pointers
        $<$<CONFIG:Release>:/Ob2> # Inline function expansion
        # $<$<CONFIG:Release>:/GL> # Whole program optimization
    )
    target_link_options(waves_field PRIVATE
        $<$<CONFIG:Release>:/LTCG> # Link-time code generation for Release
    )
endif()
