set(USE_GNUSTEP ON CACHE BOOL "Using GNUstep toolchain")

find_program(CLANG_COMPILER clang)
find_program(CLANGXX_COMPILER clang++)
if(CLANG_COMPILER AND CLANGXX_COMPILER)
    set(CMAKE_C_COMPILER ${CLANG_COMPILER})
    set(CMAKE_CXX_COMPILER ${CLANGXX_COMPILER})
    set(CMAKE_OBJC_COMPILER ${CLANG_COMPILER})
    set(CMAKE_OBJCXX_COMPILER ${CLANGXX_COMPILER})
endif()

# Default find program for GNUstep
find_program(GNUSTEP_CONFIG gnustep-config)
if(GNUSTEP_CONFIG)
    execute_process(
        COMMAND ${GNUSTEP_CONFIG} --objc-flags
        OUTPUT_VARIABLE GNUSTEP_OBJC_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    execute_process(
        COMMAND ${GNUSTEP_CONFIG} --gui-libs
        OUTPUT_VARIABLE GNUSTEP_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    # separate the string into a list for CMake
    separate_arguments(GNUSTEP_LIBS_LIST UNIX_COMMAND "${GNUSTEP_LIBS}")
    
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} ${GNUSTEP_OBJC_FLAGS}")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} ${GNUSTEP_OBJC_FLAGS}")
    # Also apply to CXX in case Objective-C++ headers are included in C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNUSTEP_OBJC_FLAGS}")
endif()
