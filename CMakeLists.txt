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

if(WIN32)
  set(PLATFORM_OS "windows")
elseif(USE_GNUSTEP)
  set(PLATFORM_OS "gnustep")
elseif(APPLE)
  set(PLATFORM_OS "macos")
elseif(UNIX)
  set(PLATFORM_OS "linux")
else()
  set(PLATFORM_OS "unknown")
endif()

set(OUT_DIR_BIN "${CMAKE_SOURCE_DIR}/bin")
set(OUT_DIR_LIB "${CMAKE_SOURCE_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUT_DIR_BIN}/$<CONFIG>/${PLATFORM_OS}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUT_DIR_LIB}/$<CONFIG>/${PLATFORM_OS}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUT_DIR_LIB}/$<CONFIG>/${PLATFORM_OS}")

foreach(config IN ITEMS Debug Release MinSizeRel RelWithDebInfo)
  string(TOUPPER ${config} CONFIG_UPPER)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${OUT_DIR_BIN}/${config}/${PLATFORM_OS}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${OUT_DIR_LIB}/${config}/${PLATFORM_OS}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${OUT_DIR_LIB}/${config}/${PLATFORM_OS}")
endforeach()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

option(WR_BUILD_APP "build the vulkan application target" OFF)
option(WR_FETCH_DEPS "fetch ImGUI is not found" ON)
option(WR_COMPILE_SHADER "compile slang shaders to SPIR-V" ON)

option(WR_WIN32_EXECUTABLE "Build as a Windows GUI subsystem executable" ON)
option(WR_DEBUG_CONSOLE "Enable debug console on Windows" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")

set(SOURCE_FILES
  main.cpp
)

include(CompilerOptions)
include(Dependencies)

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
