#pragma once

#include "mesh_types.h"
#include "platform/command_pool.h"
#include "platform/vk_device_types.h"



#include <functional>
#include <span>

#include <volk.h>

class AllocatedImage;

class ResourceUploader
{
public:
    ResourceUploader(VkDevice device, VkQueue graphicsQueue, const QueueFamilyIndices& indices, VmaAllocator allocator);

    ResourceUploader(const ResourceUploader&) = delete;
    ResourceUploader& operator=(const ResourceUploader&) = delete;
    ResourceUploader(ResourceUploader&&) noexcept = default;
    ResourceUploader& operator=(ResourceUploader&&) noexcept = default;

    ~ResourceUploader();


    void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

    AllocatedImage uploadImage(
        const void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false
    );

private:
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    CommandPool m_commandPool;
    VmaAllocator m_allocator;

    VkFence m_fence;
};