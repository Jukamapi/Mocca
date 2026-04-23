cmake_minimum_required(VERSION 3.20)
project(VulkanEngine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#options
option(ENGINE_ENABLE_VALIDATION "Enable Vulkan validation layers" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(dependencies)
include(imgui)
include(shaders)

#vendor headers
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE vendor)
target_compile_definitions(glm INTERFACE
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE vendor)

add_library(vma INTERFACE)
target_include_directories(vma INTERFACE vendor)

#engine
add_library(engine STATIC)

target_sources(engine PRIVATE
    src/engine/core/application.cpp

    src/engine/window/window.cpp

    src/engine/vk/context.cpp
    src/engine/vk/frame_manager.cpp
    src/engine/vk/instance.cpp
    src/engine/vk/physical_device.cpp
    src/engine/vk/logical_device.cpp

    src/engine/renderer/renderer.cpp

    src/engine/resources/loader.cpp
)

target_sources(engine PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/engine/core/application.h
        src/engine/core/event.h
        src/engine/core/input.h


        src/engine/window/window.h

        src/engine/vk/context.h
        src/engine/vk/utils.h
        src/engine/vk/deletion_queue.h
        src/engine/vk/frame_manager.h
        src/engine/vk/instance.h
        src/engine/vk/physical_device.h
        src/engine/vk/logical_device.h



        src/engine/renderer/renderer.h
        src/engine/renderer/render_feature.h

        src/engine/resources/loader.h
)

target_include_directories(engine PUBLIC src)

target_link_libraries(engine PUBLIC
    Vulkan::Vulkan
    SDL2::SDL2
    fastgltf
    glm
    stb
    vma
)


#app
add_executable(app src/main.cpp)
set_target_properties(app PROPERTIES WIN32_EXECUTABLE FALSE)

target_compile_definitions(app PRIVATE SDL_MAIN_HANDLED)

target_link_libraries(app PRIVATE
    engine
    SDL2::SDL2
)

#shader
add_dependencies(app shaders)