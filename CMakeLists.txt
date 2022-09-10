cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(myproject VERSION 1.0)

set(C_FLAGS "-Wall")
set(CMAKE_BUILD_TYPE Debug)
add_executable(myproject glad.c main.c)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(myproject PRIVATE "./include/")
target_include_directories(myproject PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(myproject "-framework Cocoa")
target_link_libraries(myproject "-framework OpenGL")
target_link_libraries(myproject "-framework IOKit")
target_link_libraries(myproject glfw ${OPENGL_gl_LIBRARY})
