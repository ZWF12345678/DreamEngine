cmake_minimum_required(VERSION 3.20.0)

project(Core)

aux_source_directory(Src/Windows SRC)
aux_source_directory(Src/Math SRC)
aux_source_directory(Src/Common SRC)
aux_source_directory(Src/UniversalPlatform SRC)
aux_source_directory(Src/Misc SRC)


add_library(${PROJECT_NAME} SHARED ${SRC})

#Header file definition that needs to be referenced
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${DE_RUNTIMECORE_INCLUDE_DIRS}
)

#Define some preemptive macros
target_compile_definitions(${PROJECT_NAME} PRIVATE
    KEXPORT
)

#third party dependencies

# link with spdlog
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${THIRD_PARTY_INCLUDE_DIRS_ROOT}
)

target_link_directories(${PROJECT_NAME}
    PUBLIC
    ${THIRD_PARTY_LIBS_DIR_ROOT}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    spdlog
)

