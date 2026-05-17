#pragma once

#include <volk.h>
#include <span>

class DescriptorAllocator
{
public:
    struct PoolSizeRatio
    {
        VkDescriptorType type;
        float ratio;
    };

    DescriptorAllocator() = default;
    DescriptorAllocator(VkDevice device, uint32_t maxSets, std::span<const PoolSizeRatio> poolRatios);

    ~DescriptorAllocator();

    DescriptorAllocator(const DescriptorAllocator&) = delete;
    DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;
    DescriptorAllocator(DescriptorAllocator&& other) noexcept;
    DescriptorAllocator& operator=(DescriptorAllocator&& other) noexcept;

    VkDescriptorSet allocate(VkDescriptorSetLayout layout);
    void clearDescriptors();

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkDescriptorPool m_pool{VK_NULL_HANDLE};
};