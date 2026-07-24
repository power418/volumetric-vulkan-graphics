###
#
#
#
####

add_library(vvr_compiler_options INTERFACE)

if(MSVC)
  target_compile_options(vvr_compiler_options
    INTERFACE
    /W4
    /permissive-
    /Zc:__cplusplus
  )
else()
  target_compile_options(vvr_compiler_options
    INTERFACE
    -Wall
    -Wextra
    -Wpendantic
  )
endif()
