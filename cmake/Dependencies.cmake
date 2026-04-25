include("${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")
include(FetchContent)

find_package(Vulkan REQUIRED)
find_package(glm CONFIG QUIET)
find_package(imgui CONFIG QUIET)

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
