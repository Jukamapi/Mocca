set(MOCCA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MOCCA_VENDOR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# vendor
add_library(volk STATIC ${MOCCA_VENDOR}/volk/volk.c)
target_link_libraries(volk PUBLIC Vulkan::Headers)
target_include_directories(volk PUBLIC ${MOCCA_VENDOR}/volk)
target_compile_definitions(volk PUBLIC VK_NO_PROTOTYPES)
if(UNIX)
    target_link_libraries(volk PRIVATE dl)
endif()

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${MOCCA_VENDOR})
target_compile_definitions(glm INTERFACE
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${MOCCA_VENDOR})

add_library(vma INTERFACE)
target_include_directories(vma INTERFACE ${MOCCA_VENDOR})

# engine library
add_library(mocca STATIC)

target_sources(mocca PRIVATE
    # core layer
    src/core/vk_check.cpp

    # platform layer
    src/platform/window.cpp

    src/platform/context.cpp
    src/platform/command_pool.cpp

    src/platform/context/instance.cpp
    src/platform/context/physical_device.cpp
    src/platform/context/logical_device.cpp
    src/platform/context/gpu_alloc.cpp

    src/platform/presentation/surface.cpp
    src/platform/presentation/swapchain.cpp
    src/platform/presentation/swapchain_manager.cpp

    # resource layer
    src/resource/asset_manager.cpp
    src/resource/resource_uploader.cpp

    src/resource/vulkan/allocated_image.cpp
    src/resource/vulkan/allocated_buffer.cpp
    src/resource/vulkan/descriptor_allocator.cpp
    src/resource/vulkan/descriptor_writer.cpp
    src/resource/vulkan/descriptor_allocator_growable.cpp
    src/resource/vulkan/descriptor_layout.cpp

    # rendering layer
    src/renderer/renderer.cpp
    src/renderer/frame_manager.cpp
    src/renderer/imgui_manager.cpp
    src/renderer/pipelines/pipeline.cpp
    src/renderer/pipelines/pipeline_manager.cpp
    src/renderer/pipelines/graphics_pipeline.cpp
    src/renderer/pipelines/compute_pipeline.cpp

    # app layer
    src/application/application.cpp

    # vendor init
    ${MOCCA_VENDOR}/vendor_init.cpp
)

target_sources(mocca PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${MOCCA_SRC}
    FILES
        # core util layer
        src/core/event.h
        src/core/input.h
        src/core/timer.h
        src/core/types.h

        src/core/vk_check.h
        src/core/vk_types.h


        # platform layer
        src/platform/window.h

        src/platform/command_pool.h

        src/platform/context.h
        src/platform/context/instance.h
        src/platform/context/physical_device.h
        src/platform/context/logical_device.h
        src/platform/context/gpu_alloc.h

        src/platform/presentation/surface.h
        src/platform/presentation/swapchain.h
        src/platform/presentation/swapchain_manager.h

        # resource layer
        src/resource/loader.h
        src/resource/deletion_queue.h
        src/resource/asset_manager.h
        src/resource/resource_uploader.h

        src/resource/vulkan/allocated_image.h
        src/resource/vulkan/allocated_buffer.h
        src/resource/vulkan/descriptor_allocator.h
        src/resource/vulkan/descriptor_writer.h
        src/resource/vulkan/descriptor_allocator_growable.h
        src/resource/vulkan/descriptor_layout.h

        # rendering layer
        src/renderer/renderer.h
        src/renderer/render_feature.h
        src/renderer/frame_manager.h
        src/renderer/imgui_manager.h
        src/renderer/pipelines/pipeline.h
        src/renderer/pipelines/pipeline_manager.h
        src/renderer/pipelines/graphics_pipeline.h
        src/renderer/pipelines/compute_pipeline.h

        # app layer
        src/application/application.h
)

target_include_directories(mocca PUBLIC ${MOCCA_SRC})

target_compile_definitions(mocca PUBLIC
    VK_NO_PROTOTYPES
    SHADER_DIR="${SPIRV_DIR}"
    ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
)

target_link_libraries(mocca PUBLIC
    volk
    SDL2::SDL2
    fastgltf
    glm
    stb
    vma
    imgui
)
