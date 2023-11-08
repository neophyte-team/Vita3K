set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(
        gui-qt
        STATIC
        include/gui-qt/functions.h
        include/gui-qt/state.h
        src/gui.cpp
        src/user_management.cpp

        src/initial-setup/initial_setup_wizard.cpp

        src/configuration/settings_window.cpp
        src/configuration/base_settings_widget.cpp
        src/configuration/core_settings_widget.cpp
        src/configuration/cpu_settings_widget.cpp
        src/configuration/gpu_settings_widget.cpp
        src/configuration/system_settings_widget.cpp
        src/configuration/emulator_settings_widget.cpp
        src/configuration/network_settings_widget.cpp
        src/configuration/debug_settings_widget.cpp

        src/main_window.cpp
        src/render_window.cpp
        src/live_area.cpp
        src/user_management_dialog.cpp

        src/models/app_list_model.cpp
        # list files whenever we begin to add them
)
find_package(Qt5 COMPONENTS Core Widgets)

target_include_directories(gui-qt PUBLIC include ${CMAKE_SOURCE_DIR}/vita3k)
target_link_libraries(gui-qt PUBLIC app compat config dialog emuenv https ime imgui glutil lang regmgr np)
target_link_libraries(gui-qt PRIVATE audio ctrl io kernel miniz psvpfsparser pugixml::pugixml stb renderer packages sdl2 touch vkutil host::dialog)
target_link_libraries(gui-qt PUBLIC tracy)
target_link_libraries(gui-qt PUBLIC vulkan)
target_link_libraries(gui-qt PUBLIC Qt5::Core Qt5::Widgets)