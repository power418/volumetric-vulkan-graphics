cmake_minimum_required(VERSION 3.22)
project("vk_volumetric"
  VERSION 0.1.0
  DESCRIPTION "Volumetric Rendring Side Using Vulkan"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/OutputDirectories.cmake)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(WR_BUILD_APP "build the vulkan application target" OFF)
option(WR_FETCH_DEPS "fetch ImGUI is not found" ON)
option(WR_COMPILE_SHADER "compile slang shaders to SPIR-V" ON)

option(WR_WIN32_EXECUTABLE "Build as a Windows GUI subsystem executable" ON)
option(WR_DEBUG_CONSOLE "Enable debug console on Windows" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/deps")
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/deps/cache")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")

set(SOURCE_FILES
  main.cpp
)

include(CompilerOptions)
include(Dependencies)

# ==============================================================================
# Modular Workspace (Sub-Projects)
# ==============================================================================
# Blok ini akan mencari CMakeLists.txt di setiap folder modul utama (arsitektur engine)
# dan menambahkannya ke dalam build system secara otomatis.
foreach(module IN ITEMS core platform drivers shaders utils ui apps)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${module}/CMakeLists.txt")
        add_subdirectory(${module})
    endif()
endforeach()

if(APPLE OR USE_GNUSTEP)
  enable_language(OBJCXX)
  set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE OBJCXX)
elseif(UNIX)
  find_package(X11 REQUIRED)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(Vulkan_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME}
    PRIVATE
      ${Vulkan_INCLUDE_DIRS}
  )
elseif(Vulkan_INCLUDE_DIR)
  target_include_directories(${PROJECT_NAME}
    PRIVATE
      "${Vulkan_INCLUDE_DIR}"
  )
endif()

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
    d3d11
    d3d12
    dxgi
    d3dcompiler
  )
elseif(APPLE OR USE_GNUSTEP)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      VK_USE_PLATFORM_VULKAN
  )
  
  if(USE_GNUSTEP)
    target_compile_definitions(${PROJECT_NAME} PRIVATE __APPLE__=1)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GNUSTEP_LIBS_LIST})
  else()
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(QUARZCORE_FRAMEWORK QuartzCore REQUIRED)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
  
    target_link_libraries(${PROJECT_NAME}
      PRIVATE
        ${COCOA_FRAMEWORK}
        ${QUARZCORE_FRAMEWORK}
        ${METAL_FRAMEWORK}
    )
  endif()
elseif(UNIX AND NOT APPLE)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      VK_USE_PLATFORM_XLIB_KHR
  )
  
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(XFT REQUIRED xft)
  
  target_include_directories(${PROJECT_NAME} PRIVATE ${XFT_INCLUDE_DIRS})
  
  target_link_libraries(${PROJECT_NAME} PRIVATE
    X11::X11
    ${XFT_LIBRARIES}
  )
endif()
