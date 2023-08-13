# CMakeList.txt : CMake project for OpenGLSamples, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("OpenGLSamples")

# Add source to this project's executable.

add_subdirectory(glfw)
add_subdirectory(glew)
add_subdirectory(glm)
add_subdirectory("sample1")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sample1 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
target_include_directories(sample1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
target_include_directories(sample1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glew/include)
target_include_directories(sample1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glm)
