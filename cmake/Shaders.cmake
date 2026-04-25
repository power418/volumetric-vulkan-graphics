###
#
#
#
###

find_program(
  SLANG_EXEC
  NAMES SLANG
  HINTS
    "$ENV{VULKAN_SDK}/bin"
    "$ENV{VULKAN_SDK}/bin"
)

if(NOT SLANG_EXEC)
  message(FATAL_ERROR "slang not found. make sure vulkan sdk is installed and VULKAN_SDK is set.")
endif()

set(VVR_SHADERS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(VVR_SHADER_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

file(MAKE_DIRECTORY "${VVR_SHADER_BIN_DIR")

file(GLOB VVR_SHADER_FILES
  CONFIGURE_DEPENDS
  "${VVR_SHADER_SOURCE_DIR}/*.slang"
)

set(VVR_SHADER_OUTPUTS "")

foreach(SHADER_FILE ${VVR_SHADER_FILES})
  file(RELATIVE_PATH SHADER_RELATIVE_PATH "${VVR_SHADER_SOURCE_DIR}" "${SHADER_FILE}")

  set(SHADER_OUTPUTS "${VVR_SHADER_BIN_DIR}/${SHADER_RELATIVE_PATH}.spv")

  get_filename_component(SHADER_OUTPUT_DIR "${SHADER_OUTPUT_FILE}" DIRECTORY)
  file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

  add_custom_command(
    OUTPUT "${SHADER_OUTPUT_FILE}"
    COMMAND "${SLANG_EXEC}" "${SHADER_FILE}" -o "${SHADER_OUTPUT_FILE}"
    DEPENDS "${SHADER_FILE}"
    COMMENT "compile shader ${SHADER_RELATIVE_PATH"
    VERBATIM
  )
endforeach()

add_custom_target(volumetric_shaders ALL
  DEPENDS ${VVR_SHADER_OUTPUTS}
)
