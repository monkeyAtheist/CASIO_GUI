# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(GUI)

# CASIO GUI frozen public API release.
set(CASIO_GUI_VERSION "1.0.0")

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)
find_package(Gint 2.9 REQUIRED)

set(CASIO_GUI_EXAMPLE "" CACHE STRING "Build src/examples/<name>.cpp instead of src/main.cpp")

if(CASIO_GUI_EXAMPLE)
  set(APP_MAIN "src/examples/${CASIO_GUI_EXAMPLE}.cpp")

  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_MAIN}")
    message(FATAL_ERROR "Unknown CASIO_GUI_EXAMPLE: ${CASIO_GUI_EXAMPLE}")
  endif()
else()
  set(APP_MAIN "src/main.cpp")
endif()

set(SOURCES
  ${APP_MAIN}
  src/CASIO_GUI/cursor/cursor.cpp
  src/CASIO_GUI/item/item.cpp
  src/CASIO_GUI/gui/gui.cpp
  src/CASIO_GUI/screenshot/screenshot.cpp
  src/CASIO_GUI/lib/libc_stubs.c
  # ...
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
  # ...
)
set(ASSETS_fx
  assets-fx/example.png
  assets-fx/gui_test_image.png
)
set(ASSETS_cg
  assets-cg/example.png
  assets-cg/gui_test_image.png
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_executable(GUI ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})

target_compile_options(GUI PRIVATE 
-Wall 
-Wextra 
-Os 
-g 
-flto
-std=c++17
-fno-freestanding
)

target_link_libraries(GUI PRIVATE
    Gint::Gint
    stdc++
)


if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET GUI OUTPUT "GUI.g1a"
    NAME "GUI" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET GUI OUTPUT "GUI.g3a"
    NAME "GUI" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET GUI OUTPUT "GUI-fx.g3a"
    NAME "GUI-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET GUI OUTPUT "GUI-hh2.bin")
endif()
