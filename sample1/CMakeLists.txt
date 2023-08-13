cmake_minimum_required(VERSION 3.8)


include_directories("common")


file(GLOB_RECURSE COMMON_SOURCES "common/*")
file(GLOB SHADERS "shaders/*")

add_executable(sample1 sample1.cpp ${COMMON_SOURCES} ${SHADERS})
file(COPY ${SHADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)


target_link_libraries(sample1
	glfw
	libglew_static
)