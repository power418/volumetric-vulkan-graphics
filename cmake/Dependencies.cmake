include("${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")
include(FetchContent)

set(WR_VULKAN_SDK_ROOT "" CACHE PATH "Optional Vulkan SDK root directory")

if(WIN32 AND NOT WR_VULKAN_SDK_ROOT)
  if(DEFINED ENV{VULKAN_SDK} AND EXISTS "$ENV{VULKAN_SDK}/Include/vulkan/vulkan.h")
    file(TO_CMAKE_PATH "$ENV{VULKAN_SDK}" WR_VULKAN_SDK_ROOT_DEFAULT)
  elseif(EXISTS "C:/VulkanSDK/1.4.350.0/Include/vulkan/vulkan.h")
    set(WR_VULKAN_SDK_ROOT_DEFAULT "C:/VulkanSDK/1.4.350.0")
  endif()

  if(DEFINED WR_VULKAN_SDK_ROOT_DEFAULT)
    set(WR_VULKAN_SDK_ROOT "${WR_VULKAN_SDK_ROOT_DEFAULT}" CACHE PATH "Optional Vulkan SDK root directory" FORCE)
  endif()
endif()

if(WR_VULKAN_SDK_ROOT)
  list(PREPEND CMAKE_PREFIX_PATH "${WR_VULKAN_SDK_ROOT}")

  if(NOT Vulkan_INCLUDE_DIR AND EXISTS "${WR_VULKAN_SDK_ROOT}/Include/vulkan/vulkan.h")
    set(Vulkan_INCLUDE_DIR "${WR_VULKAN_SDK_ROOT}/Include" CACHE PATH "Vulkan SDK include directory")
  endif()

  if(WIN32 AND NOT Vulkan_LIBRARY AND EXISTS "${WR_VULKAN_SDK_ROOT}/Lib/vulkan-1.lib")
    set(Vulkan_LIBRARY "${WR_VULKAN_SDK_ROOT}/Lib/vulkan-1.lib" CACHE FILEPATH "Vulkan loader library")
  endif()
endif()

find_package(Vulkan REQUIRED)
find_package(glm CONFIG QUIET)
find_package(imgui CONFIG QUIET)
find_package(GNUstep QUIET)

if(NOT glm_FOUND)
  if(WR_FETCH_DEPS)
    message(STATUS "glm not found. fetching GLM...")

    FetchContent_Declare(
      glm
      GIT_REPOSITORY https://github.com/g-truc/glm.git
      GIT_TAG 1.0.1
      GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(glm)
  else()
    message(FATAL_ERROR "glm not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

if(TARGET glm AND NOT TARGET glm::glm)
  add_library(glm::glm ALIAS glm)
endif()

if(NOT TARGET imgui AND NOT TARGET imgui::imgui)
  if(WR_FETCH_DEPS)
    message(STATUS "imgui not found. fetching Dear ImGui...")

    CPMAddPackage(
      NAME imgui
      GITHUB_REPOSITORY ocornut/imgui
      GIT_TAG v1.91.8
      GIT_SHALLOW TRUE
      DOWNLOAD_ONLY YES
    )

    if(imgui_SOURCE_DIR)
      add_library(imgui STATIC
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
      )

      target_include_directories(imgui
        PUBLIC
          "${imgui_SOURCE_DIR}"
          "${imgui_SOURCE_DIR}/backends"
      )
    endif()
  else()
    message(FATAL_ERROR "imgui not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

if(TARGET imgui::imgui AND NOT TARGET imgui)
  add_library(imgui ALIAS imgui::imgui)
endif()

if(TARGET imgui AND NOT TARGET imgui::imgui)
  add_library(imgui::imgui ALIAS imgui)
endif()
