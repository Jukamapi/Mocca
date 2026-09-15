add_executable(sandbox
    src/main.cpp

    src/sandbox_app.h
    src/sandbox_app.cpp

    src/experiments/imgui_feature.h
    src/experiments/imgui_feature.cpp

    src/experiments/test_feature.h
    src/experiments/test_feature.cpp

    src/experiments/triangle_feature.h
    src/experiments/triangle_feature.cpp

    src/experiments/mesh_feature.h
    src/experiments/mesh_feature.cpp
)
target_include_directories(sandbox PRIVATE src)

set_target_properties(sandbox PROPERTIES WIN32_EXECUTABLE FALSE)
target_compile_definitions(sandbox PRIVATE SDL_MAIN_HANDLED)

target_link_libraries(sandbox PRIVATE
    mocca
    SDL2::SDL2
)

add_dependencies(sandbox shaders)