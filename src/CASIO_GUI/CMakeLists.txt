# ==============================================================================
# CASIO_GUI
# Static library for CASIO fxSDK / gint
# ==============================================================================

cmake_minimum_required(VERSION 3.15)

# ------------------------------------------------------------------------------
# Library
# ------------------------------------------------------------------------------

add_library(CASIO_GUI STATIC

    cursor/cursor.cpp
    item/item.cpp
    gui/gui.cpp
    screenshot/screenshot.cpp

    lib/libc_stubs.c
)

# ------------------------------------------------------------------------------
# Alias
#
# Allows:
#   target_link_libraries(... CASIO_GUI::CASIO_GUI)
# ------------------------------------------------------------------------------

add_library(CASIO_GUI::CASIO_GUI ALIAS CASIO_GUI)

# ------------------------------------------------------------------------------
# Public include directory
#
# With this configuration, application code can use for example:
#
#   #include <gui/gui.hpp>
#   #include <item/item.hpp>
#   #include <cursor/cursor.hpp>
# ------------------------------------------------------------------------------

target_include_directories(
    CASIO_GUI

    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ------------------------------------------------------------------------------
# Language standards
# ------------------------------------------------------------------------------

set_target_properties(
    CASIO_GUI

    PROPERTIES

    C_STANDARD 11
    C_STANDARD_REQUIRED ON

    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# ------------------------------------------------------------------------------
# Compiler options
# ------------------------------------------------------------------------------

target_compile_options(
    CASIO_GUI

    PUBLIC

    $<$<COMPILE_LANGUAGE:CXX>:-fno-freestanding>
)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

target_link_libraries(
    CASIO_GUI

    PUBLIC

    Gint::Gint
)

# ------------------------------------------------------------------------------
# Version information
# ------------------------------------------------------------------------------

target_compile_definitions(
    CASIO_GUI

    PUBLIC

    CASIO_GUI_VERSION_MAJOR=1
    CASIO_GUI_VERSION_MINOR=0
    CASIO_GUI_VERSION_PATCH=0
)
