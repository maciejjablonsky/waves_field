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
find_package(
    Qt6
    REQUIRED
    COMPONENTS Core
               Quick
               Gui
               Qml
)

qt_standard_project_setup(REQUIRES 6.7)
set(CMAKE_AUTOMOC ON)

add_executable(waves_field)

set_target_properties(
    waves_field
    PROPERTIES AUTOMOC ON
               AUTORCC ON
               AUTOUIC ON
)

# qt_add_resources(waves_field "resources" FILES qml/main.qml )

qt_add_qml_module(
    waves_field
    URI app
    QML_FILES qml/Main.qml
    RESOURCES assets/Button-Pedal-Checked.png
              assets/Button-Pedal-Checked@2x.png
              assets/Button-Pedal.png
              assets/Button-Pedal@2x.png
              assets/Guitar-Pedal-Background.png
              assets/Guitar-Pedal-Background@2x.png
              assets/Knob-Dial.png
              assets/Knob-Dial@2x.png
              assets/Knob-Markings.png
              assets/Knob-Markings@2x.png
              assets/LED-Checked.png
              assets/LED-Checked@2x.png
              assets/LED.png
              assets/LED@2x.png
              assets/Screw.png
              assets/Screw@2x.png
              assets/Switch-Checked.png
              assets/Switch-Checked@2x.png
              assets/Switch.png
              assets/Switch@2x.png
              fonts/Prisma.ttf
              fonts/RussoOne-Regular.ttf
)

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
           Qt6::Core
           Qt6::Quick
           Qt6::Gui
           Qt6::Qml
)

add_dependencies(waves_field wf_shaders)
target_compile_features(waves_field PUBLIC cxx_std_23)

target_sources(
    waves_field
    PUBLIC src/main.cpp
    # src/utils.cpp src/vk/vk.cpp ${_qt_resources_sources} PUBLIC FILE_SET
    # CXX_MODULES FILES src/utils.ixx src/systems.ixx src/systems/input.ixx
    # src/callback_interfaces.ixx src/systems/pc_input.ixx
    # src/systems/system.ixx src/systems/key.ixx src/commands/commands.ixx
    # src/systems/render.ixx src/systems/window.ixx src/vk/vk.ixx
    # src/systems/gui.ixx src/components/render.ixx src/components/transform.ixx
)
