cmake_minimum_required(VERSION 3.30)

project(grk_triangle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(deps)

add_executable(main main.cpp program.cpp)
target_compile_options(main PRIVATE -Wall -Wextra -pedantic -DGLFW_INCLUDE_NONE)
target_link_libraries(main glfw glad)
