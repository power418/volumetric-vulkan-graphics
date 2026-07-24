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
find_package(GTest QUIET)

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

if(NOT GTest_FOUND AND NOT TARGET gtest)
  if(WR_FETCH_DEPS)
    message(STATUS "GTest not found. fetching GoogleTest...")
    CPMAddPackage(
      NAME googletest
      GITHUB_REPOSITORY google/googletest
      GIT_TAG v1.14.0
      OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
    )
  else()
    message(WARNING "GTest not found. install it or enable WR_FETCH_DEPS to support unit testing.")
  endif()
endif()

# --- ZLIB ---
find_package(ZLIB QUIET)
if(NOT ZLIB_FOUND AND NOT TARGET ZLIB::ZLIB)
  if(WR_FETCH_DEPS)
    message(STATUS "ZLIB not found. fetching ZLIB...")
    CPMAddPackage(
      NAME zlib
      GITHUB_REPOSITORY madler/zlib
      GIT_TAG v1.3.1
    )
  else()
    message(FATAL_ERROR "ZLIB not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- enet ---
find_package(enet CONFIG QUIET)
if(NOT enet_FOUND AND NOT TARGET enet)
  if(WR_FETCH_DEPS)
    message(STATUS "enet not found. fetching enet...")
    CPMAddPackage(
      NAME enet
      GITHUB_REPOSITORY lsalzman/enet
      GIT_TAG v1.3.18
    )
  else()
    message(FATAL_ERROR "enet not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- stb ---
if(NOT TARGET stb::stb)
  if(WR_FETCH_DEPS)
    message(STATUS "fetching stb...")
    CPMAddPackage(
      NAME stb
      GITHUB_REPOSITORY nothings/stb
      GIT_TAG master
      DOWNLOAD_ONLY YES
    )
    if(stb_SOURCE_DIR)
      add_library(stb INTERFACE)
      target_include_directories(stb INTERFACE "${stb_SOURCE_DIR}")
      add_library(stb::stb ALIAS stb)
    endif()
  else()
    message(WARNING "stb not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- Skia ---
find_package(Skia QUIET)
if(NOT Skia_FOUND AND NOT TARGET Skia::Skia)
  message(STATUS "Skia not found. Note: Skia typically requires custom build steps or prebuilt binaries.")
  if(WR_FETCH_DEPS)
    message(WARNING "Fetching Skia directly via CMake is complex. You may need to provide prebuilt binaries or a dedicated CMake wrapper for Skia.")
  endif()
endif()

# --- CUDA / OptiX (For DLSS / FSR support) ---
find_package(CUDAToolkit QUIET)
if(CUDAToolkit_FOUND)
  message(STATUS "CUDA Toolkit found: ${CUDAToolkit_VERSION} - Ready for DLSS/FSR integration")
else()
  message(STATUS "CUDA Toolkit not found. DLSS/FSR features may be disabled.")
endif()

find_package(OptiX QUIET)
if(OptiX_FOUND)
  message(STATUS "OptiX found: ${OptiX_VERSION} - Ready for OptiX integration")
else()
  message(STATUS "OptiX not found.")
endif()

# --- xsimd ---
find_package(xsimd CONFIG QUIET)
if(NOT xsimd_FOUND AND NOT TARGET xsimd)
  if(WR_FETCH_DEPS)
    message(STATUS "xsimd not found. fetching xsimd...")
    CPMAddPackage(
      NAME xsimd
      GITHUB_REPOSITORY xtensor-stack/xsimd
      GIT_TAG 13.0.0
    )
  else()
    message(FATAL_ERROR "xsimd not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- nlohmann/json ---
find_package(nlohmann_json CONFIG QUIET)
if(NOT nlohmann_json_FOUND AND NOT TARGET nlohmann_json::nlohmann_json)
  if(WR_FETCH_DEPS)
    message(STATUS "nlohmann_json not found. fetching nlohmann_json...")
    CPMAddPackage(
      NAME nlohmann_json
      GITHUB_REPOSITORY nlohmann/json
      GIT_TAG v3.11.3
    )
  else()
    message(FATAL_ERROR "nlohmann_json not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- xxHash ---
find_package(xxHash CONFIG QUIET)
if(NOT xxHash_FOUND AND NOT TARGET xxHash::xxhash)
  if(WR_FETCH_DEPS)
    message(STATUS "xxHash not found. fetching xxHash...")
    CPMAddPackage(
      NAME xxHash
      GITHUB_REPOSITORY Cyan4973/xxHash
      GIT_TAG v0.8.2
    )
  else()
    message(FATAL_ERROR "xxHash not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- miniaudio ---
if(NOT TARGET miniaudio::miniaudio)
  if(WR_FETCH_DEPS)
    message(STATUS "fetching miniaudio...")
    CPMAddPackage(
      NAME miniaudio
      GITHUB_REPOSITORY mackron/miniaudio
      GIT_TAG master
      DOWNLOAD_ONLY YES
    )
    if(miniaudio_SOURCE_DIR)
      add_library(miniaudio INTERFACE)
      target_include_directories(miniaudio INTERFACE "${miniaudio_SOURCE_DIR}")
      add_library(miniaudio::miniaudio ALIAS miniaudio)
    endif()
  else()
    message(WARNING "miniaudio not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- Tracy ---
find_package(Tracy CONFIG QUIET)
if(NOT Tracy_FOUND AND NOT TARGET Tracy::TracyClient)
  if(WR_FETCH_DEPS)
    message(STATUS "Tracy not found. fetching Tracy...")
    CPMAddPackage(
      NAME tracy
      GITHUB_REPOSITORY wolfpld/tracy
      GIT_TAG v0.11.1
      OPTIONS
        "TRACY_ENABLE ON"
        "TRACY_ON_DEMAND ON"
    )
  else()
    message(WARNING "Tracy not found. install it or enable WR_FETCH_DEPS to support profiling")
  endif()
endif()

# --- Clay ---
if(NOT TARGET clay::clay)
  if(WR_FETCH_DEPS)
    message(STATUS "fetching clay...")
    CPMAddPackage(
      NAME clay
      GITHUB_REPOSITORY nicbarker/clay
      GIT_TAG main
      DOWNLOAD_ONLY YES
    )
    if(clay_SOURCE_DIR)
      add_library(clay INTERFACE)
      target_include_directories(clay INTERFACE "${clay_SOURCE_DIR}")
      add_library(clay::clay ALIAS clay)
    endif()
  else()
    message(WARNING "clay not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- nanosvg ---
if(NOT TARGET nanosvg::nanosvg)
  if(WR_FETCH_DEPS)
    message(STATUS "fetching nanosvg...")
    CPMAddPackage(
      NAME nanosvg
      GITHUB_REPOSITORY memononen/nanosvg
      GIT_TAG master
      DOWNLOAD_ONLY YES
    )
    if(nanosvg_SOURCE_DIR)
      add_library(nanosvg INTERFACE)
      target_include_directories(nanosvg INTERFACE "${nanosvg_SOURCE_DIR}/src")
      add_library(nanosvg::nanosvg ALIAS nanosvg)
    endif()
  else()
    message(WARNING "nanosvg not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- tinygltf ---
if(NOT TARGET tinygltf::tinygltf)
  if(WR_FETCH_DEPS)
    message(STATUS "fetching tinygltf...")
    CPMAddPackage(
      NAME tinygltf
      GITHUB_REPOSITORY syoyo/tinygltf
      GIT_TAG release
      DOWNLOAD_ONLY YES
    )
    if(tinygltf_SOURCE_DIR)
      add_library(tinygltf INTERFACE)
      target_include_directories(tinygltf INTERFACE "${tinygltf_SOURCE_DIR}")
      add_library(tinygltf::tinygltf ALIAS tinygltf)
    endif()
  else()
    message(WARNING "tinygltf not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- VulkanMemoryAllocator (VMA) ---
if(NOT TARGET VulkanMemoryAllocator::VulkanMemoryAllocator)
  if(WR_FETCH_DEPS)
    message(STATUS "fetching VulkanMemoryAllocator...")
    CPMAddPackage(
      NAME VulkanMemoryAllocator
      GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
      GIT_TAG master
      DOWNLOAD_ONLY YES
    )
    if(VulkanMemoryAllocator_SOURCE_DIR)
      add_library(VulkanMemoryAllocator INTERFACE)
      target_include_directories(VulkanMemoryAllocator INTERFACE "${VulkanMemoryAllocator_SOURCE_DIR}/include")
      add_library(VulkanMemoryAllocator::VulkanMemoryAllocator ALIAS VulkanMemoryAllocator)
    endif()
  else()
    message(WARNING "VulkanMemoryAllocator not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()

# --- DirectX Headers ---
if(NOT TARGET DirectX-Headers)
  if(WR_FETCH_DEPS)
    message(STATUS "fetching DirectX-Headers...")
    CPMAddPackage(
      NAME DirectX-Headers
      GITHUB_REPOSITORY microsoft/DirectX-Headers
      GIT_TAG v1.614.1
    )
  else()
    message(WARNING "DirectX-Headers not found. install it or enable WR_FETCH_DEPS")
  endif()
endif()
