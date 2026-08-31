#include "resource_uploader.h"

#include "core/vk_check.h"

ResourceUploader::ResourceUploader(
    VkDevice device, VkQueue graphicsQueue, const QueueFamilyIndices& indices, VmaAllocator allocator
)
    : m_device(device),
      m_graphicsQueue(graphicsQueue),
      m_commandPool(indices, m_device),
      m_allocator(allocator)
{
    m_commandPool.allocateBuffers(1);

    VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = 0};

    VK_CHECK(vkCreateFence(m_device, &fenceInfo, nullptr, &m_fence));
}

void ResourceUploader::immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
{
    VK_CHECK(vkResetFences(m_device, 1, &m_fence));
    m_commandPool.reset();

    VkCommandBuffer cmd = m_commandPool.getBuffers()[0];

    VkCommandBufferBeginInfo cmdBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    function(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    VkCommandBufferSubmitInfo cmdSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = cmd
    };

    VkSubmitInfo2 submit{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdSubmitInfo,
    };

    VK_CHECK(vkQueueSubmit2(m_graphicsQueue, 1, &submit, m_fence));
    VK_CHECK(vkWaitForFences(m_device, 1, &m_fence, true, 9999999999));
}


GPUMeshBuffers ResourceUploader::uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices)
{
    const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
    const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

    AllocatedBuffer index{
        m_allocator,
        indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY
    };

    AllocatedBuffer vertex{
        m_allocator,
        vertexBufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY
    };


    AllocatedBuffer staging{
        m_allocator,
        vertexBufferSize + indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_MEMORY_USAGE_CPU_ONLY
    };

    void* data = staging.getMappedData();

    std::memcpy(data, vertices.data(), vertexBufferSize);
    std::memcpy(static_cast<char*>(data) + vertexBufferSize, indices.data(), indexBufferSize);

    immediateSubmit(
        [&](VkCommandBuffer cmd)
        {
            VkBufferCopy vertexCopy{.srcOffset = 0, .dstOffset = 0, .size = vertexBufferSize};

            vkCmdCopyBuffer(cmd, staging.getBuffer(), vertex.getBuffer(), 1, &vertexCopy);

            VkBufferCopy indexCopy{.srcOffset = vertexBufferSize, .dstOffset = 0, .size = indexBufferSize};
            vkCmdCopyBuffer(cmd, staging.getBuffer(), index.getBuffer(), 1, &indexCopy);
        }
    );

    VkBufferDeviceAddressInfo deviceAdressInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = vertex.getBuffer()
    };

    VkDeviceAddress vertexAddress = vkGetBufferDeviceAddress(m_device, &deviceAdressInfo);


    GPUMeshBuffers newSurface{std::move(index), std::move(vertex), vertexAddress};

    return newSurface;
}

ResourceUploader::~ResourceUploader()
{
    if(m_fence != VK_NULL_HANDLE)
        vkDestroyFence(m_device, m_fence, nullptr);
}