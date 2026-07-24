set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_OBJC_COMPILER clang)
set(CMAKE_OBJCXX_COMPILER clang++)

# Default find program for GNUstep
find_program(GNUSTEP_CONFIG gnustep-config)
if(GNUSTEP_CONFIG)
    execute_process(
        COMMAND ${GNUSTEP_CONFIG} --objc-flags
        OUTPUT_VARIABLE GNUSTEP_OBJC_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} ${GNUSTEP_OBJC_FLAGS}")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} ${GNUSTEP_OBJC_FLAGS}")
    # Also apply to CXX in case Objective-C++ headers are included in C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNUSTEP_OBJC_FLAGS}")
endif()
