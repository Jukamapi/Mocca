add_executable(sandbox main.cpp)

set_target_properties(sandbox PROPERTIES WIN32_EXECUTABLE FALSE)
target_compile_definitions(sandbox PRIVATE SDL_MAIN_HANDLED)

target_link_libraries(sandbox PRIVATE
    mocca
    SDL2::SDL2
)

add_dependencies(sandbox shaders)