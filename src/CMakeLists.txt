cmake_minimum_required(VERSION 3.8)


include_directories("common")

file(GLOB_RECURSE COMMON_SOURCES "common/*")
file(GLOB SHADERS "shaders/*")
file(GLOB TEXTURES "textures/*")


add_executable(src "main.cpp" ${COMMON_SOURCES} ${SHADERS})
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(COPY ${TEXTURES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/textures)


target_link_libraries(
	src
	glfw
	libglew_static
)