#
# RMG-Input CMakeLists.txt
#
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Gui Widgets Core REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)


set(RMG_INPUT_SOURCES
    UserInterface/Widget/ControllerWidget.ui
    UserInterface/Widget/ControllerWidget.cpp
    UserInterface/Widget/CustomButton.cpp
    UserInterface/MainDialog.ui
    UserInterface/MainDialog.cpp
    UserInterface/UIResources.qrc
    main.cpp
)

if (CMAKE_BUILD_TYPE MATCHES Release)
    add_executable(RMG-Input WIN32 ${RMG_INPUT_SOURCES})
else()
    add_executable(RMG-Input ${RMG_INPUT_SOURCES})
endif()

target_link_libraries(RMG-Input ${SDL2_LIBRARIES})

target_include_directories(RMG-Input PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})

target_link_libraries(RMG-Input Qt5::Gui Qt5::Widgets)
