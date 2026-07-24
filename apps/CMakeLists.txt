# This file serves as the main entry point to build sub-projects located 
# within the apps/ directory.

# We use if(EXISTS ...) to ensure CMake configures successfully even if the 
# respective CMakeLists.txt files have not yet been created in the subdirectories.
# Once created, CMake will automatically detect and include them.

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/engine/CMakeLists.txt")
    add_subdirectory(engine)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/components/CMakeLists.txt")
    add_subdirectory(components)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/editor/CMakeLists.txt")
    add_subdirectory(editor)
endif()
