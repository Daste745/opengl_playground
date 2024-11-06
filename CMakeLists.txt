cmake_minimum_required(VERSION 3.30)

project(grk_triangle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(deps)

set(IMGUI_SOURCES
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(main main.cpp program.cpp ${IMGUI_SOURCES})
target_include_directories(main PRIVATE deps/imgui deps/imgui/backends)
target_compile_options(main PRIVATE -Wall -Wextra -pedantic -DGLFW_INCLUDE_NONE)
target_link_libraries(main glfw glad)
