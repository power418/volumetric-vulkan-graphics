# Mencari seluruh file source dan header secara otomatis di dalam folder platform beserta sub-foldernya
file(GLOB_RECURSE PLATFORM_SOURCES
    "*.cpp"
    "*.c"
    "*.m"
    "*.mm"
    "*.h"
)

# Membangun platform_driver sebagai library statis
add_library(platform_driver STATIC ${PLATFORM_SOURCES})

# Mengekspos direktori root project (atau direktori platform) 
# agar `#include <platform/...>` bisa dipanggil dari modul lain
target_include_directories(platform_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories(platform_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# ========================
# OS-Specific Dependencies
# ========================
if(WIN32)
    # Kebutuhan dasar Win32 API (Window, Input, dll)
    target_link_libraries(platform_driver PUBLIC
        user32
        gdi32
        shell32
        ole32
    )
elseif(APPLE OR USE_GNUSTEP)
    if(USE_GNUSTEP)
        # Menghubungkan library GNUstep di Linux/Windows
        target_link_libraries(platform_driver PUBLIC ${GNUSTEP_LIBS_LIST})
    else()
        # Menghubungkan Framework Cocoa di macOS asli
        find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
        find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
        target_link_libraries(platform_driver PUBLIC 
            ${COCOA_FRAMEWORK}
            ${QUARTZCORE_FRAMEWORK}
        )
    endif()
elseif(UNIX AND NOT APPLE)
    # Kebutuhan dasar X11 di Linux
    find_package(X11 REQUIRED)
    target_link_libraries(platform_driver PUBLIC
        X11::X11
    )
endif()
