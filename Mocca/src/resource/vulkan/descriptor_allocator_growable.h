#pragma once

#include <volk.h>
#include <span>
#include <vector>

class DescriptorAllocatorGrowable
{
public:
    struct PoolSizeRatio
    {
        VkDescriptorType type;
        float ratio;
    };

    DescriptorAllocatorGrowable() = default;
    DescriptorAllocatorGrowable(VkDevice device, uint32_t maxSets, std::span<const PoolSizeRatio> poolRatios);

    ~DescriptorAllocatorGrowable();

    DescriptorAllocatorGrowable(const DescriptorAllocatorGrowable&) = delete;
    DescriptorAllocatorGrowable& operator=(const DescriptorAllocatorGrowable&) = delete;
    DescriptorAllocatorGrowable(DescriptorAllocatorGrowable&& other) noexcept;
    DescriptorAllocatorGrowable& operator=(DescriptorAllocatorGrowable&& other) noexcept;

    VkDescriptorSet allocate(VkDescriptorSetLayout layout, void* pNext);
    void clearPools();

private:
    VkDescriptorPool getPool();
    VkDescriptorPool createPool();
    void destroyPools();

    VkDevice m_device{VK_NULL_HANDLE};
    std::vector<PoolSizeRatio> m_ratios;
    std::vector<VkDescriptorPool> m_fullPools;
    std::vector<VkDescriptorPool> m_readyPools;
    uint32_t m_setsPerPool;
};