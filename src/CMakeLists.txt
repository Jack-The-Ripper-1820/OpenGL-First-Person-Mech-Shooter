cmake_minimum_required(VERSION 3.8)

include_directories("common")

file(GLOB_RECURSE COMMON_SOURCES "common/*")
file(GLOB SHADERS "shaders/*")
file(GLOB TEXTURES "textures/*")
file(GLOB MODELS "models/*")

add_executable(src "main.cpp" ${COMMON_SOURCES} ${SHADERS})



file(COPY ${COMMON_SOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/common)

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/shaders)
    add_custom_command(TARGET src POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders
    )
endif()
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/textures)
    add_custom_command(TARGET src POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/textures
    )
endif()
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/models)
    add_custom_command(TARGET src POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/models
    )
endif()

add_custom_command(TARGET src POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/textures ${CMAKE_CURRENT_BINARY_DIR}/textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models ${CMAKE_CURRENT_BINARY_DIR}/models
)


target_link_libraries(
    src
    glfw
    libglew_static
    assimp::assimp
)
