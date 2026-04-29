cmake_minimum_required(VERSION 3.26)
project(minigin)

# Enable platform detection for MSVC
if(NOT CMAKE_SYSTEM_NAME)
  set(CMAKE_SYSTEM_NAME Windows)
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_SYSTEM_PROCESSOR AMD64)
  else()
    set(CMAKE_SYSTEM_PROCESSOR x86)
  endif()
endif()

# Add Windows definitions early
if(WIN32)
  add_compile_definitions(_WINDOWS _WINDLL _USRDLL)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions(_WIN64 _AMD64_)
  else()
    add_compile_definitions(_X86_)
  endif()
endif()

if(EMSCRIPTEN)
  message("Using Emscripten!")
endif()

include(FetchContent)

# add glm
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
  FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_MakeAvailable(glm)
endif()

# Add SDL3
find_package(SDL3 3.4 CONFIG QUIET)
if(NOT SDL3_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3
      URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3)
    set(SDL3_DIR "${SDL3_SOURCE_DIR}/cmake")
    find_package(SDL3 CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-3.4.0
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(SDL3)
  endif()
endif()

# Add SDL3_ttf
find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3_ttf
      URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    set(SDL3_ttf_DIR "${SDL3_ttf_SOURCE_DIR}/cmake")
    find_package(SDL3_ttf CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG release-3.2.2
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    set(SDL3TTF_INSTALL OFF)
    if(EMSCRIPTEN)
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten")
      set(SDL3TTF_USE_SYSTEM_FREETYPE ON)
      set(SDL3TTF_VENDORED OFF)
      set(SDLTTF_PLUTOSVG OFF)
      add_compile_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
      add_link_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
    endif()
    FetchContent_MakeAvailable(SDL3_ttf)
  endif()
endif()

# Add SDL3_mixer
find_package(SDL3_mixer CONFIG QUIET)
if(NOT SDL3_mixer_FOUND)
  FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG release-3.2.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  set(SDL3MIXER_INSTALL OFF)
  FetchContent_MakeAvailable(SDL3_mixer)
endif()

# Dear ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.92.5-docking
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(
  imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp)

if(DEFINED SDL3_SOURCE_DIR)
  target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${SDL3_SOURCE_DIR}/include)
else()
  target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
endif()

target_link_libraries(imgui PUBLIC SDL3::SDL3)
target_compile_features(imgui PUBLIC cxx_std_20)
target_compile_definitions(imgui PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

FetchContent_Declare(
  imgui_plot
  GIT_REPOSITORY https://github.com/soulthreads/imgui-plot.git
  GIT_TAG v0.1.0
  PATCH_COMMAND ${CMAKE_COMMAND} -E echo "Patching imgui_plot for ImGui compatibility..."
)
FetchContent_MakeAvailable(imgui_plot)

# Patch imgui_plot for newer ImGui API
file(READ "${imgui_plot_SOURCE_DIR}/src/imgui_plot.cpp" IMGUI_PLOT_CONTENT)
string(REPLACE "ItemHoverable(frame_bb, id)" "ItemHoverable(frame_bb, id, ImGuiItemFlags_None)" IMGUI_PLOT_CONTENT "${IMGUI_PLOT_CONTENT}")
file(WRITE "${imgui_plot_SOURCE_DIR}/src/imgui_plot.cpp" "${IMGUI_PLOT_CONTENT}")

# Configure imgui_plot to use imgui
target_include_directories(imgui_plot PUBLIC ${imgui_SOURCE_DIR})
target_link_libraries(imgui_plot PUBLIC imgui)

find_package(VLD CONFIG)

# Add steam if available
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/steam")
find_package(Steamworks QUIET)

add_subdirectory(Minigin)
add_subdirectory(Digger)

if(WIN32 AND NOT EMSCRIPTEN)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT digger)
endif()
