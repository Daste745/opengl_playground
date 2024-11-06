add_subdirectory(glad)
add_subdirectory(glfw)

set(IMGUI_SOURCES
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_opengl3.cpp
    PARENT_SCOPE
)
set(IMGUI_INCLUDE_DIRS
    deps/imgui
    deps/imgui/backends
    PARENT_SCOPE
)
