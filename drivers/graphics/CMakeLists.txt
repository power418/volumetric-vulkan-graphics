set(GRAPHICS_SOURCES
    dx/dx_context.cpp
    dx/dx_context.h
    gl/gl_context.cpp
    gl/gl_context.h
    mtl/mtl_context.cpp
    mtl/mtl_context.h
    vk/vk_context.cpp
    vk/vk_context.h
)

# Memaksa file .cpp dikompilasi sebagai Objective-C++ 
# agar syntax #import dan tipe data dari framework Apple (id, MTLDevice) bisa dikenali
set_source_files_properties(mtl/mtl_context.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")

add_library(graphics_driver STATIC ${GRAPHICS_SOURCES})

# Include all subdirectories so they can include their headers easily
target_include_directories(graphics_driver PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/dx"
    "${CMAKE_CURRENT_SOURCE_DIR}/gl"
    "${CMAKE_CURRENT_SOURCE_DIR}/mtl"
    "${CMAKE_CURRENT_SOURCE_DIR}/vk"
)

# Common dependencies
target_link_libraries(graphics_driver PUBLIC utils)

# ========================
# Vulkan Setup
# ========================
target_link_libraries(graphics_driver PUBLIC
    Vulkan::Vulkan
    VulkanMemoryAllocator
)

# ========================
# OpenGL Setup
# ========================
if(WIN32)
    target_link_libraries(graphics_driver PRIVATE opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(graphics_driver PRIVATE OpenGL::GL)
endif()

# ========================
# DirectX Setup
# ========================
if(WIN32)
    target_link_libraries(graphics_driver PRIVATE
        d3d12
        d3d11
        dxgi
        d3dcompiler
        DirectX-Headers
    )
else()
    find_library(VKD3D_LIB vkd3d REQUIRED)
    find_path(VKD3D_INCLUDE_DIR vkd3d.h PATH_SUFFIXES vkd3d)

    target_link_libraries(graphics_driver PRIVATE
        ${VKD3D_LIB}
        DirectX-Headers
    )
    if(VKD3D_INCLUDE_DIR)
        target_include_directories(graphics_driver PRIVATE ${VKD3D_INCLUDE_DIR})
    endif()
endif()

# ========================
# Metal Setup
# ========================
if(APPLE)
    find_library(METAL_LIBRARY Metal)
    target_link_libraries(graphics_driver PRIVATE ${METAL_LIBRARY})
endif()
