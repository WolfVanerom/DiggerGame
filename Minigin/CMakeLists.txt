set(MINIGIN_SOURCES
  Component.cpp
  Font.cpp
  GameObject.cpp
  Minigin.cpp
  Observer.cpp
  Renderer.cpp
  ResourceManager.cpp
  Scene.cpp
  SceneManager.cpp
  ServiceLocator.cpp
  TextComponent.cpp
  Texture2D.cpp
  TextureComponent.cpp
  Transform.cpp
)

add_library(minigin STATIC ${MINIGIN_SOURCES})

target_compile_features(minigin PUBLIC cxx_std_20)

if(WIN32)
  target_link_libraries(minigin PRIVATE xinput.lib)
endif()

target_include_directories(minigin PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/Digger
  ${CMAKE_SOURCE_DIR}/Digger/Player
  ${CMAKE_SOURCE_DIR}/Digger/Level
  ${glm_SOURCE_DIR}
  ${glm_SOURCE_DIR}/glm
  ${SDL3_SOURCE_DIR}/include
  ${SDL3_ttf_SOURCE_DIR}/include
  ${SDL3_mixer_SOURCE_DIR}/include
  ${imgui_SOURCE_DIR}
  ${imgui_plot_SOURCE_DIR}/src
)

target_link_libraries(minigin PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  SDL3_mixer::SDL3_mixer
  glm::glm
  imgui
  imgui_plot
)

if(VLD_FOUND)
  target_include_directories(minigin PRIVATE ${VLD_INCLUDE_DIRS})
  target_link_libraries(minigin PRIVATE ${VLD_LIBRARIES})
endif()

if(MSVC)
  target_compile_options(minigin PRIVATE /W4 /WX)
else()
  target_compile_options(minigin PRIVATE -Wall -Wextra -Werror)
endif()

if(Steamworks_FOUND)
  target_link_libraries(minigin PUBLIC Steamworks::Steamworks)
  target_compile_definitions(minigin PRIVATE USE_STEAMWORKS)
endif()
