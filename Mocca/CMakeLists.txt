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
    # engine
    src/engine/application.cpp
    src/engine/imgui_manager.cpp

    # platform
    src/platform/window.cpp

    # vulkan
    src/platform/vulkan/core/context.cpp
    src/platform/vulkan/core/instance.cpp
    src/platform/vulkan/core/physical_device.cpp
    src/platform/vulkan/core/logical_device.cpp
    src/platform/vulkan/core/surface.cpp
    src/platform/vulkan/core/gpu_alloc.cpp
    src/platform/vulkan/utils/vk_check.cpp
    src/platform/vulkan/resources/swapchain.cpp
    src/platform/vulkan/resources/swapchain_manager.cpp
    src/platform/vulkan/resources/allocated_image.cpp
    src/platform/vulkan/resources/descriptor_allocator.cpp
    src/platform/vulkan/resources/descriptor_layout.cpp
    src/platform/vulkan/pipeline/pipeline.cpp
    src/platform/vulkan/pipeline/graphics_pipeline.cpp
    src/platform/vulkan/pipeline/compute_pipeline.cpp
    src/platform/vulkan/commands/command_pool.cpp
    src/platform/vulkan/commands/frame_manager.cpp

    # renderer
    src/engine/renderer.cpp

    # vendor init
    ${MOCCA_VENDOR}/vendor_init.cpp
)

target_sources(mocca PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${MOCCA_SRC}
    FILES
        # core
        src/core/event.h
        src/core/input.h
        src/core/timer.h
        src/core/types.h

        # engine
        src/engine/application.h
        src/engine/renderer.h
        src/engine/render_feature.h
        src/engine/imgui_manager.h

        # platform
        src/platform/window.h
        src/platform/vulkan/core/context.h
        src/platform/vulkan/core/instance.h
        src/platform/vulkan/core/physical_device.h
        src/platform/vulkan/core/logical_device.h
        src/platform/vulkan/core/surface.h
        src/platform/vulkan/core/gpu_alloc.h
        src/platform/vulkan/utils/vk_check.h
        src/platform/vulkan/utils/vk_types.h
        src/platform/vulkan/resources/swapchain.h
        src/platform/vulkan/resources/swapchain_manager.h
        src/platform/vulkan/resources/allocated_image.h
        src/platform/vulkan/resources/descriptor_allocator.h
        src/platform/vulkan/resources/descriptor_layout.h
        src/platform/vulkan/pipeline/pipeline.h
        src/platform/vulkan/pipeline/graphics_pipeline.h
        src/platform/vulkan/pipeline/compute_pipeline.h
        src/platform/vulkan/commands/command_pool.h
        src/platform/vulkan/commands/frame_manager.h
        src/platform/vulkan/resources/deletion_queue.h

        # resource
        src/resource/loader.h
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
