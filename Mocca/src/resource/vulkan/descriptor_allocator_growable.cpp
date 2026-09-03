#include "descriptor_allocator_growable.h"

#include "core/vk_check.h"

DescriptorAllocatorGrowable::DescriptorAllocatorGrowable(
    VkDevice device, uint32_t maxSets, std::span<const PoolSizeRatio> poolRatios
)
    : m_device(device),
      m_setsPerPool(maxSets)
{
    for(auto r : poolRatios)
    {
        m_ratios.push_back(r);
    }

    VkDescriptorPool newPool = createPool();

    m_setsPerPool = maxSets * 1.5;

    m_readyPools.push_back(newPool);
}

VkDescriptorPool DescriptorAllocatorGrowable::getPool()
{
    VkDescriptorPool newPool;
    if(m_readyPools.size() != 0)
    {
        newPool = m_readyPools.back();
        m_readyPools.pop_back();
    }
    else
    {
        // need to create a new pool
        newPool = createPool();

        m_setsPerPool = m_setsPerPool * 1.5;
        if(m_setsPerPool > 4092)
        {
            m_setsPerPool = 4092;
        }
    }

    return newPool;
}

VkDescriptorPool DescriptorAllocatorGrowable::createPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    for(PoolSizeRatio ratio : m_ratios)
    {
        poolSizes.push_back(
            VkDescriptorPoolSize{.type = ratio.type, .descriptorCount = uint32_t(ratio.ratio * m_setsPerPool)}
        );
    }

    VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = m_setsPerPool,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    VkDescriptorPool newPool;

    VK_CHECK(vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &newPool));

    return newPool;
}

void DescriptorAllocatorGrowable::clearPools()
{
    for(auto p : m_readyPools)
    {
        VK_CHECK(vkResetDescriptorPool(m_device, p, 0));
    }
    for(auto p : m_fullPools)
    {
        VK_CHECK(vkResetDescriptorPool(m_device, p, 0));
        m_readyPools.push_back(p);
    }
    m_fullPools.clear();
}

void DescriptorAllocatorGrowable::destroyPools()
{
    for(auto p : m_readyPools)
    {
        vkDestroyDescriptorPool(m_device, p, nullptr);
    }
    m_readyPools.clear();

    for(auto p : m_fullPools)
    {
        vkDestroyDescriptorPool(m_device, p, nullptr);
    }
    m_fullPools.clear();
}

VkDescriptorSet DescriptorAllocatorGrowable::allocate(VkDescriptorSetLayout layout, void* pNext)
{
    VkDescriptorPool poolToUse = getPool();

    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = pNext,
        .descriptorPool = poolToUse,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };

    VkDescriptorSet ds;
    VkResult result = vkAllocateDescriptorSets(m_device, &allocInfo, &ds);

    // allocation failed, try again
    if(result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
    {

        m_fullPools.push_back(poolToUse);

        poolToUse = getPool();
        allocInfo.descriptorPool = poolToUse;

        VK_CHECK(vkAllocateDescriptorSets(m_device, &allocInfo, &ds));
    }

    m_readyPools.push_back(poolToUse);
    return ds;
}

DescriptorAllocatorGrowable& DescriptorAllocatorGrowable::operator=(DescriptorAllocatorGrowable&& other) noexcept
{
    if(this != &other)
    {
        destroyPools();

        m_device = other.m_device;
        m_setsPerPool = other.m_setsPerPool;
        m_ratios = std::move(other.m_ratios);
        m_fullPools = std::move(other.m_fullPools);
        m_readyPools = std::move(other.m_readyPools);

        other.m_device = VK_NULL_HANDLE;
        other.m_setsPerPool = 0;
    }

    return *this;
}

DescriptorAllocatorGrowable::DescriptorAllocatorGrowable(DescriptorAllocatorGrowable&& other) noexcept
    : m_device(other.m_device),
      m_setsPerPool(other.m_setsPerPool),
      m_ratios(std::move(other.m_ratios)),
      m_fullPools(std::move(other.m_fullPools)),
      m_readyPools(std::move(other.m_readyPools))
{
    other.m_device = VK_NULL_HANDLE;
    other.m_setsPerPool = 0;
}

DescriptorAllocatorGrowable::~DescriptorAllocatorGrowable()
{
    destroyPools();
}