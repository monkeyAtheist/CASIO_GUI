# ==============================================================================
# CASIO application
# fxSDK / gint / METTRE_LE_NOM_DU_PROJET
#
# Build:
#   fxsdk build-fx
#   fxsdk build-cg
# ==============================================================================

cmake_minimum_required(VERSION 3.15)

project(
    TEMP PROJECT NAME

    VERSION 1.0.0

    LANGUAGES
        C
        CXX
)

# ------------------------------------------------------------------------------
# Application configuration
# ------------------------------------------------------------------------------

set(APP_TARGET "${PROJECT_NAME}")
set(APP_NAME "METTRE_LE_NOM_DU_PROJET")

# ------------------------------------------------------------------------------
# fxSDK
# ------------------------------------------------------------------------------

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)

find_package(Gint 2.9 REQUIRED)

# ------------------------------------------------------------------------------
# CASIO_GUI
# ------------------------------------------------------------------------------

add_subdirectory(src/CASIO_GUI)

# ------------------------------------------------------------------------------
# Optional example
# ------------------------------------------------------------------------------

set(
    CASIO_GUI_EXAMPLE
    ""
    CACHE STRING
    "Build src/examples/<name>.cpp instead of src/main.cpp"
)

if(CASIO_GUI_EXAMPLE)

    set(
        APP_MAIN
        "src/examples/${CASIO_GUI_EXAMPLE}.cpp"
    )

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_MAIN}")
        message(
            FATAL_ERROR
            "Unknown CASIO_GUI_EXAMPLE: ${CASIO_GUI_EXAMPLE}"
        )
    endif()

else()

    set(
        APP_MAIN
        "src/main.cpp"
    )

endif()

# ------------------------------------------------------------------------------
# Application sources
# ------------------------------------------------------------------------------

set(
    SOURCES

    ${APP_MAIN}

    # Application files:
    #
    # src/game.cpp
    # src/menu.cpp
    # src/player.cpp
)

# ------------------------------------------------------------------------------
# Assets
# ------------------------------------------------------------------------------

set(
    ASSETS

    # Common assets
)

set(
    ASSETS_fx

    # assets-fx/example.png
)

set(
    ASSETS_cg

    # assets-cg/example.png
)

fxconv_declare_assets(
    ${ASSETS}
    ${ASSETS_fx}
    ${ASSETS_cg}

    WITH_METADATA
)

# ------------------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------------------

add_executable(
    ${APP_TARGET}

    ${SOURCES}
    ${ASSETS}
    ${ASSETS_${FXSDK_PLATFORM}}
)

# ------------------------------------------------------------------------------
# Include paths
# ------------------------------------------------------------------------------

target_include_directories(
    ${APP_TARGET}

    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ------------------------------------------------------------------------------
# C / C++ standard
# ------------------------------------------------------------------------------

set_target_properties(
    ${APP_TARGET}

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
    ${APP_TARGET}

    PRIVATE

    -Wall
    -Wextra
    -Os
    -g
    -flto
)

target_link_options(
    ${APP_TARGET}

    PRIVATE

    -flto
)

# ------------------------------------------------------------------------------
# Libraries
# ------------------------------------------------------------------------------

target_link_libraries(
    ${APP_TARGET}

    PRIVATE

    CASIO_GUI::CASIO_GUI

    Gint::Gint

    stdc++
)

# ------------------------------------------------------------------------------
# Add-in generation
# ------------------------------------------------------------------------------

if("${FXSDK_PLATFORM_LONG}" STREQUAL "fx9860G")

    generate_g1a(
        TARGET ${APP_TARGET}

        OUTPUT "${APP_NAME}.g1a"

        NAME "${APP_NAME}"

        ICON
            assets-fx/icon.png
    )

elseif("${FXSDK_PLATFORM_LONG}" STREQUAL "fxCG50")

    generate_g3a(
        TARGET ${APP_TARGET}

        OUTPUT "${APP_NAME}.g3a"

        NAME "${APP_NAME}"

        ICONS
            assets-cg/icon-uns.png
            assets-cg/icon-sel.png
    )

elseif("${FXSDK_PLATFORM_LONG}" STREQUAL "fx9860G_G3A")

    generate_g3a(
        TARGET ${APP_TARGET}

        OUTPUT "${APP_NAME}-fx.g3a"

        NAME "${APP_NAME}-fx"

        ICONS
            assets-cg/icon-uns.png
            assets-cg/icon-sel.png
    )

elseif("${FXSDK_PLATFORM_LONG}" STREQUAL "fxCP")

    generate_hh2_bin(
        TARGET ${APP_TARGET}

        OUTPUT "${APP_NAME}-hh2.bin"
    )

else()

    message(
        WARNING
        "Unknown fxSDK platform: ${FXSDK_PLATFORM_LONG}"
    )

endif()
