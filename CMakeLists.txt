cmake_minimum_required(VERSION 3.22)
project("vk_volumetric"
  VERSION 1.0.0
  DESCRIPTION "Volumetric Rendring Side Using Vulkan"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

option(WR_BUILD_APP "build the vulkan application target" OFF)
option(WR_FETCH_DEPS "fetch ImGUI is not found" ON)
option(WR_COMPILE_SHADER "compile slang shaders to SPIR-V" ON)

option(WR_WIN32_EXECUTABLE "Build as a Windows GUI subsystem executable" ON)
option(WR_DEBUG_CONSOLE "Enable debug console on Windows" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")

include(CompilerOptions)
include(Dependencies)

if(WIN32)
  find_package(OpenGL REQUIRED)
elseif(NOT APPLE)
  find_package(X11 REQUIRED)
else()
  enable_language(OBJCXX)
endif()

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

file(GLOB ALL_INCLUDE_ITEMS
  CONFIGURE_DEPENDS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/*"
)

foreach(item IN LISTS ALL_INCLUDE_ITEMS)
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
      AND NOT item MATCHES "^(build|Debug|Release|\\..*|cmake|docs)$")
    target_include_directories(${PROJECT_NAME} 
      PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
    )
  endif()
endforeach()


target_link_libraries(${PROJECT_NAME}
  PRIVATE
  imgui::imgui
  glm::glm
  OpenGL::GL
  Vulkan::Vulkan
)

if(WIN32)
  if(WR_DEBUG_CONSOLE)
    target_compile_definitions(${PROJECT_NAME}
      PRIVATE
        WR_DEBUG_CONSOLE=1
    )
  endif()

  if(WR_WIN32_EXECUTABLE)
    target_compile_definitions(${PROJECT_NAME}
      PRIVATE
        WR_WIN32_EXECUTABLE=1
    )
  endif()

  set_target_properties(${PROJECT_NAME}
    PROPERTIES
      WIN32_EXECUTABLE ${WR_WIN32_EXECUTABLE}
  )
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      VK_USE_PLATFORM_WIN32_KHR
      WIN32_LEAN_AND_MEAN
      NOMINMAX
  )
  target_link_libraries(${PROJECT_NAME} PRIVATE
    user32
    gdi32
    shell32
    ole32
  )
elseif(APPLE)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      VK_USE_PLATFORM_VULKAN
  )
  find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
  find_library(QUARZCORE_FRAMEWORK QuartzCore REQUIRED)
  find_library(METAL_FRAMEWORK Metal REQUIRED)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      ${COCOA_FRAMEWORK}
      ${QUARZCORE_FRAMEWORK}
      ${METAL_FRAMEWORK}
  )
elseif(UNIX AND NOT APPLE)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      VK_USE_PLATFORM_XLIB_KHR
  )
  target_link_libraries(${PROJECT_NAME} PRIVATE
    X11::X11
  )
endif()
