set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(
        gui-qt
        STATIC
        include/gui-qt/functions.h
        include/gui-qt/state.h
        src/gui_qt.cpp

        src/main_window.h
        src/render_window.h
        src/initial_setup_wizard.h
        src/select_language_page.h
        src/select_pref_path_page.h
        src/install_firmware_page.h
        src/live_area.h
        src/main_window.cpp
        src/render_window.cpp
        src/initial_setup_wizard.cpp
        src/select_language_page.cpp
        src/select_pref_path_page.cpp
        src/install_firmware_page.cpp
        src/live_area.cpp

        src/models/app_list_model.h
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