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
    src/core/vulkan/vk_check.cpp

    # platform layer
    src/platform/window.cpp

    src/platform/vulkan/context.cpp
    src/platform/vulkan/command_pool.cpp

    src/platform/vulkan/context/instance.cpp
    src/platform/vulkan/context/physical_device.cpp
    src/platform/vulkan/context/logical_device.cpp
    src/platform/vulkan/context/gpu_alloc.cpp

    src/platform/vulkan/presentation/surface.cpp
    src/platform/vulkan/presentation/swapchain.cpp
    src/platform/vulkan/presentation/swapchain_manager.cpp

    # resource layer
    src/resource/vulkan/allocated_image.cpp
    src/resource/vulkan/allocated_buffer.cpp
    src/resource/vulkan/descriptor_allocator.cpp
    src/resource/vulkan/descriptor_layout.cpp

    # rendering layer
    src/renderer/renderer.cpp
    src/renderer/vulkan/frame_manager.cpp
    src/renderer/vulkan/imgui_manager.cpp
    src/renderer/vulkan/pipelines/pipeline.cpp
    src/renderer/vulkan/pipelines/pipeline_manager.cpp
    src/renderer/vulkan/pipelines/graphics_pipeline.cpp
    src/renderer/vulkan/pipelines/compute_pipeline.cpp

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

        src/core/vulkan/vk_check.h
        src/core/vulkan/vk_types.h


        # platform layer
        src/platform/window.h

        src/platform/vulkan/command_pool.h

        src/platform/vulkan/context.h
        src/platform/vulkan/context/instance.h
        src/platform/vulkan/context/physical_device.h
        src/platform/vulkan/context/logical_device.h
        src/platform/vulkan/context/gpu_alloc.h

        src/platform/vulkan/presentation/surface.h
        src/platform/vulkan/presentation/swapchain.h
        src/platform/vulkan/presentation/swapchain_manager.h

        # resource layer
        src/resource/loader.h
        src/resource/deletion_queue.h

        src/resource/vulkan/allocated_image.h
        src/resource/vulkan/allocated_buffer.h
        src/resource/vulkan/descriptor_allocator.h
        src/resource/vulkan/descriptor_layout.h

        # rendering layer
        src/renderer/renderer.h
        src/renderer/render_feature.h
        src/renderer/vulkan/frame_manager.h
        src/renderer/vulkan/imgui_manager.h
        src/renderer/vulkan/pipelines/pipeline.h
        src/renderer/vulkan/pipelines/pipeline_manager.h
        src/renderer/vulkan/pipelines/graphics_pipeline.h
        src/renderer/vulkan/pipelines/compute_pipeline.h

        # app layer
        src/application/application.h
)

target_include_directories(mocca PUBLIC ${MOCCA_SRC})

target_compile_definitions(mocca PUBLIC
    VK_NO_PROTOTYPES
    SHADER_DIR="${SPIRV_DIR}"
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
