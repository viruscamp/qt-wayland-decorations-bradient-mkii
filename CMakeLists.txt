cmake_minimum_required(VERSION 3.5)

project(qt-wayland-decorations-bradient-mkii VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(USE_QT6 "Use Qt6 instead of Qt5" OFF)
option(HAS_QT6_SUPPORT "Enabling Qt Wayland decoration shadows support" OFF) 

if (NOT USE_QT6)
    find_package(QT NAMES Qt5 REQUIRED Core)
    find_package(Qt5 REQUIRED Core Gui WaylandClient XkbCommonSupport)
else()
    find_package(QT NAMES Qt6 REQUIRED Core)
    find_package(Qt6 REQUIRED Core Gui WaylandClient)
endif()

if (HAS_QT6_SUPPORT OR USE_QT6)
    message(STATUS "Enabling Qt Wayland decoration shadows support")
    message(STATUS "NOTE: This support requires changes in Qt Wayland from Qt 6.2")
    add_definitions(-DHAS_QT6_SUPPORT)
endif()

set(PROJECT_SOURCES
    main.cpp
)

set(OUT_NAME bradient-mkii)
add_library(${OUT_NAME} MODULE
    ${PROJECT_SOURCES}
)

target_link_libraries(${OUT_NAME} PRIVATE
    Qt::Core
    Qt::CorePrivate
    Qt::Gui
    Qt::GuiPrivate
    Qt::WaylandClient
    Qt::WaylandClientPrivate
)

if (NOT USE_QT6)
target_link_libraries(${OUT_NAME} PRIVATE
    Qt::XkbCommonSupport
    Qt::XkbCommonSupportPrivate
)
endif()

if (NOT QT_PLUGINS_DIR)
    if (NOT USE_QT6)
        get_target_property(REAL_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE}
                            IMPORTED_LOCATION)
        execute_process(COMMAND "${REAL_QMAKE_EXECUTABLE}" -query QT_INSTALL_PLUGINS
                        OUTPUT_VARIABLE QT_PLUGINS_DIR
                        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        set(QT_PLUGINS_DIR ${CMAKE_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS})
    endif()
endif()

install(TARGETS ${OUT_NAME} DESTINATION ${QT_PLUGINS_DIR}/wayland-decoration-client)
