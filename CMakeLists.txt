cmake_minimum_required(VERSION 3.30.0 FATAL_ERROR)

# set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
# set(CMAKE_CXX_MODULE_STD 1)

project(waves_field LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "ON")

add_subdirectory(shaders)

find_package(glfw3 REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(VulkanLoader REQUIRED CONFIG)
find_package(magic_enum REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)
find_package(stb REQUIRED CONFIG)
find_package(tinyobjloader REQUIRED CONFIG)
find_package(EnTT REQUIRED CONFIG)
find_package(Boost REQUIRED CONFIG)
find_package(Microsoft.GSL REQUIRED CONFIG)
find_package(mp-units REQUIRED CONFIG)

add_subdirectory(src/ui)

add_executable(waves_field)

target_link_libraries(
    waves_field
    PUBLIC glfw
           glm::glm
           Vulkan::Loader
           magic_enum::magic_enum
           fmt::fmt
           stb::stb
           tinyobjloader::tinyobjloader
           EnTT::EnTT
           Boost::boost
           Microsoft.GSL::GSL
           mp-units::mp-units
           wf_ui
)

add_dependencies(waves_field wf_shaders)
target_compile_features(waves_field PUBLIC cxx_std_23)

target_sources(
    waves_field
    PUBLIC src/main.cpp
            src/utils.cpp
           src/vk/vk.cpp
    PUBLIC FILE_SET
            CXX_MODULES FILES
            src/utils.ixx
            src/vk/vk.ixx
#        src/systems.ixx
           # src/systems/input.ixx src/callback_interfaces.ixx
           # src/systems/pc_input.ixx src/systems/system.ixx src/systems/key.ixx
           # src/commands/commands.ixx src/systems/render.ixx
           # src/systems/window.ixx
    # src/systems/gui.ixx src/components/render.ixx src/components/transform.ixx
)
