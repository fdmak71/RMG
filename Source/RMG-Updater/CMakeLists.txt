#
# Rosalie's Mupen GUI Updater CMakeLists.txt
#
set(CMAKE_CXX_STANDARD 17)

set(RMG_UPDATER_SOURCES
    main.cpp
)

if (CMAKE_BUILD_TYPE MATCHES Release)
    add_executable(RMG-Updater WIN32 ${RMG_UPDATER_SOURCES})
else()
    add_executable(RMG-Updater ${RMG_UPDATER_SOURCES})
endif()
