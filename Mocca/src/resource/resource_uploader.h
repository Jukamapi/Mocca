#pragma once

#include "core/vk_types.h"
#include "platform/command_pool.h"


#include <functional>
#include <span>

#include <volk.h>

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

    void initDefaultData();

private:
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    CommandPool m_commandPool;
    VmaAllocator m_allocator;

    VkFence m_fence;
};