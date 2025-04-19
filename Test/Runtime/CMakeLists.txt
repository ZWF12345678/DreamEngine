cmake_minimum_required(VERSION 3.20.0)
project(RuntimeTest)

aux_source_directory(./ SRC)


add_executable(${PROJECT_NAME} ${SRC})

# target_include_directories(${PROJECT_NAME}
#     PUBLIC
#     ${DE_RUNTIMECORE_INCLUDE_DIRS}
# )

# target_link_libraries(${PROJECT_NAME}
#     PUBLIC
#     Core
# )

#link with spdlog

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

# target_compile_definitions(${PROJECT_NAME} PRIVATE
#     KIMPORT
# )

