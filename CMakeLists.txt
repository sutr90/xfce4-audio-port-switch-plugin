cmake_minimum_required(VERSION 3.3)
set(PROJECT_NAME port-switch)
project(${PROJECT_NAME})

set(SOURCE_FILES sample.c cairo.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(libxfce4panel REQUIRED libxfce4panel-2.0)
pkg_check_modules(libxfce4ui REQUIRED libxfce4ui-2)
pkg_check_modules(libxfce4util REQUIRED libxfce4util-1.0)
pkg_check_modules(librsvg REQUIRED librsvg-2.0)

include_directories(
        ${GTK3_INCLUDE_DIRS}
        ${libxfce4panel_INCLUDE_DIRS}
        ${libxfce4ui_INCLUDE_DIRS}
        ${libxfce4util_INCLUDE_DIRS}
        ${librsvg_INCLUDE_DIRS})

link_directories(
        ${GTK3_LIBRARY_DIRS}
        ${libxfce4panel_LIBRARY_DIRS}
        ${libxfce4ui_LIBRARY_DIRS}
        ${libxfce4util_LIBRARY_DIRS}
        ${librsvg_LIBRARY_DIRS})

add_definitions(
        ${GTK3_CFLAGS_OTHER}
        ${libxfce4panel_CFLAGS_OTHER}
        ${libxfce4ui_CFLAGS_OTHER}
        ${libxfce4util_CFLAGS_OTHER}
        ${librsvg_CFLAGS_OTHER})

add_library(${PROJECT_NAME} SHARED
        sample.c
        cairo.c)

target_link_libraries(${PROJECT_NAME}
        ${GTK3_LIBRARIES}
        ${libxfce4panel_LIBRARIES}
        ${libxfce4ui_LIBRARIES}
        ${libxfce4util_LIBRARIES}
        ${librsvg_LIBRARIES})
