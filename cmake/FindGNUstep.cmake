# cmake/FindGNUstep.cmake
# Find GNUstep installation and its flags/libraries

find_program(GNUSTEP_CONFIG gnustep-config)

if(GNUSTEP_CONFIG)
    execute_process(
        COMMAND ${GNUSTEP_CONFIG} --objc-flags
        OUTPUT_VARIABLE GNUSTEP_OBJC_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    execute_process(
        COMMAND ${GNUSTEP_CONFIG} --base-libs
        OUTPUT_VARIABLE GNUSTEP_BASE_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    execute_process(
        COMMAND ${GNUSTEP_CONFIG} --gui-libs
        OUTPUT_VARIABLE GNUSTEP_GUI_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Make them list formats so CMake can properly handle them when passing to target_link_libraries
    separate_arguments(GNUSTEP_BASE_LIBS NATIVE_COMMAND "${GNUSTEP_BASE_LIBS}")
    separate_arguments(GNUSTEP_GUI_LIBS NATIVE_COMMAND "${GNUSTEP_GUI_LIBS}")

    set(GNUstep_FOUND TRUE)
else()
    set(GNUstep_FOUND FALSE)
    if(GNUstep_FIND_REQUIRED)
        message(FATAL_ERROR "gnustep-config not found. Please install GNUstep.")
    else()
        message(WARNING "gnustep-config not found.")
    endif()
endif()
