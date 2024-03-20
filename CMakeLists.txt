cmake_minimum_required(VERSION 3.5)

project(qt-wayland-decorations-bradient-mkii VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (NOT DEFINED USE_QT6)
    find_package(QT NAMES Qt5 REQUIRED Core)
    find_package(Qt5 REQUIRED Core Gui WaylandClient XkbCommonSupport)
else()
    find_package(QT NAMES Qt6 REQUIRED Core)
    find_package(Qt6 REQUIRED Core Gui WaylandClient)
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

if (NOT DEFINED USE_QT6)
target_link_libraries(${OUT_NAME} PRIVATE
    Qt::XkbCommonSupport
    Qt::XkbCommonSupportPrivate
)
endif()

install(TARGETS ${OUT_NAME} DESTINATION ${QT_PLUGINS_DIR}/wayland-decoration-client)
