cmake_minimum_required(VERSION 3.20.0)

project(Core)

include_directories(${DE_RUNTIMECORE_INCLUDE_DIRS})

aux_source_directory(Src/Windows SRC)
aux_source_directory(Src/Math SRC)
aux_source_directory(Src/Common SRC)
aux_source_directory(Src/UniversalPlatform SRC)


add_library(${PROJECT_NAME} SHARED ${SRC})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    KEXPORT
)

