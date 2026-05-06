add_library(imgui STATIC
    Mocca/vendor/imgui/imgui.cpp
    Mocca/vendor/imgui/imgui_draw.cpp
    Mocca/vendor/imgui/imgui_tables.cpp
    Mocca/vendor/imgui/imgui_widgets.cpp
    Mocca/vendor/imgui/imgui_demo.cpp
    Mocca/vendor/imgui/backends/imgui_impl_sdl2.cpp
    Mocca/vendor/imgui/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC
    Mocca/vendor/imgui
    Mocca/vendor/imgui/backends
)

target_link_libraries(imgui PUBLIC
    SDL2::SDL2
    volk
)