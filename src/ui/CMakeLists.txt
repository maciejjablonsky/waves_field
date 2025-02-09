find_package(
    Qt6
    REQUIRED
    COMPONENTS Core
               Quick
               Gui
               Qml
)

qt_standard_project_setup(REQUIRES 6.7)

qt_add_library(wf_ui STATIC)

set(module_uri_ WfUI)

set_source_files_properties(qml/Units.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(
    wf_ui
    URI ${module_uri_}
    QML_FILES qml/Root.qml
              qml/MainMenu.qml
              qml/Units.qml
              qml/Simulation.qml
              qml/Bounds.qml
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${module_uri_}
)

target_sources(
    wf_ui
    PUBLIC FILE_SET
           CXX_MODULES
           FILES
           ui.ixx
    PRIVATE FILE_SET
            HEADERS
            BASE_DIRS
            include
            FILES
            include/ui/window.h
)

target_link_libraries(
    wf_ui
    PRIVATE Qt6::Core
            Qt6::Quick
            Qt6::Gui
            Qt6::Qml
            wf_uiplugin
)
