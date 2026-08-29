add_executable(sandbox
    src/main.cpp
    src/sandbox_app.h
    src/experiments/imgui_feature.h
    src/experiments/test_feature.h
    src/experiments/triangle_feature.h
    src/experiments/mesh_feature.h
)
target_include_directories(sandbox PRIVATE src)

set_target_properties(sandbox PROPERTIES WIN32_EXECUTABLE FALSE)
target_compile_definitions(sandbox PRIVATE SDL_MAIN_HANDLED)

target_link_libraries(sandbox PRIVATE
    mocca
    SDL2::SDL2
)

add_dependencies(sandbox shaders)