cmake_minimum_required(VERSION 3.20.0)

project(DreamEngine
    VERSION 0.0.1
    DESCRIPTION "A simple game engine"
    HOMEPAGE_URL "https://github.com/ZWF12345678/DreamEngine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

# set variables

# static library output root directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/static_libs)
# dynamic library output root directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# executable root output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# set include directories of the different mouldes
set(DE_RUNTIMECORE_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/Engine/Source/Runtime/Core/include
)

# add subdirectories
add_subdirectory(Engine/Source/Runtime/Core)


