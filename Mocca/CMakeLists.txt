add_library(mocca STATIC)

target_sources(mocca PRIVATE
    core/application.cpp
    platform/window.cpp

    vulkan/core/context.cpp
    vulkan/core/instance.cpp
    vulkan/core/physical_device.cpp
    vulkan/core/logical_device.cpp
    vulkan/core/surface.cpp

    vulkan/vk_check.cpp

    vulkan/resources/swapchain.cpp
    vulkan/resources/swapchain_manager.cpp

    vulkan/pipeline/pipeline.cpp

    vulkan/commands/command_pool.cpp
    vulkan/commands/frame_manager.cpp

    renderer/renderer.cpp

    ../../vendor/vendor_init.cpp
)

target_sources(mocca PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_SOURCE_DIR}/src
    FILES
        core/application.h
        core/event.h
        core/input.h
        core/types.h
        platform/window.h

        vulkan/core/context.h
        vulkan/core/instance.h
        vulkan/core/physical_device.h
        vulkan/core/logical_device.h
        vulkan/core/surface.h

        vulkan/vk_check.h
        vulkan/vma_alloc.h

        vulkan/resources/swapchain.h
        vulkan/resources/swapchain_manager.h

        vulkan/pipeline/pipeline.h

        vulkan/commands/command_pool.h
        vulkan/commands/frame_manager.h
        vulkan/commands/deletion_queue.h

        loaders/loader.h

        renderer/renderer.h
        renderer/render_feature.h
)

target_include_directories(mocca PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(mocca PUBLIC
    volk
    SDL2::SDL2
    fastgltf
    glm
    stb
    vma
)