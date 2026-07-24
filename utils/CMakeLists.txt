file(GLOB UTILS_SOURCES "*.cpp" "*.h")

add_library(utils STATIC ${UTILS_SOURCES})

# Jika suatu saat hanya ada header (.h), kita set linker language agar CMake tidak bingung
set_target_properties(utils PROPERTIES LINKER_LANGUAGE CXX)

# Agar bisa di-include secara global dari folder mana pun via #include "utils/logger.h"
# atau #include <utils/logger.h>
target_include_directories(utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")