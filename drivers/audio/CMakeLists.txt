set(AUDIO_SOURCES
    audio_context.cpp
    audio_context.h
)

add_library(audio_driver STATIC ${AUDIO_SOURCES})

target_include_directories(audio_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Link miniaudio
target_link_libraries(audio_driver PUBLIC
    miniaudio::miniaudio
    utils
)

# Platform-specific native API dependencies for audio
if(WIN32)
    # Miniaudio uses WASAPI, DirectSound, and WinMM on Windows.
    # Most of these are dynamically loaded, but ole32 is required for COM.
    target_link_libraries(audio_driver PRIVATE
        ole32
        uuid
    )
elseif(APPLE)
    # Miniaudio requires Apple's CoreAudio and related frameworks
    find_library(CORE_AUDIO_FRAMEWORK CoreAudio REQUIRED)
    find_library(AUDIO_UNIT_FRAMEWORK AudioUnit REQUIRED)
    find_library(AUDIO_TOOLBOX_FRAMEWORK AudioToolbox REQUIRED)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

    target_link_libraries(audio_driver PRIVATE
        ${CORE_AUDIO_FRAMEWORK}
        ${AUDIO_UNIT_FRAMEWORK}
        ${AUDIO_TOOLBOX_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
    )
elseif(UNIX AND NOT APPLE)
    # On Linux, miniaudio dynamically loads ALSA, PulseAudio, and JACK via dlopen.
    # Therefore, we need to link pthread, math (m), and dynamic loader (dl).
    target_link_libraries(audio_driver PRIVATE
        pthread
        m
        dl
    )
endif()
