#include "platform/vulkan/resources/descriptor_allocator.h"
#include "platform/vulkan/vk_check.h"

#include <vector>

DescriptorAllocator::DescriptorAllocator(VkDevice device, uint32_t maxSets, std::span<const PoolSizeRatio> poolRatios)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    for(PoolSizeRatio ratio : poolRatios)
    {
        poolSizes.push_back(
            VkDescriptorPoolSize{.type = ratio.type, .descriptorCount = uint32_t(ratio.ratio * maxSets)}
        );
    }

    VkDescriptorPoolCreateInfo pool_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = maxSets,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &m_pool));
}

VkDescriptorSet DescriptorAllocator::allocate(VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = m_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };

    VkDescriptorSet ds;
    VK_CHECK(vkAllocateDescriptorSets(m_device, &allocInfo, &ds));

    return ds;
}

DescriptorAllocator& DescriptorAllocator::operator=(DescriptorAllocator&& other) noexcept
{
    if(this != &other)
    {
        if(m_pool != VK_NULL_HANDLE)
            vkDestroyDescriptorPool(m_device, m_pool, nullptr);

        m_device = other.m_device;
        m_pool = other.m_pool;

        other.m_device = VK_NULL_HANDLE;
        other.m_pool = VK_NULL_HANDLE;
    }

    return *this;
}

DescriptorAllocator::DescriptorAllocator(DescriptorAllocator&& other) noexcept
    : m_device(other.m_device), m_pool(other.m_pool)
{
    other.m_device = VK_NULL_HANDLE;
    other.m_pool = VK_NULL_HANDLE;
}

void DescriptorAllocator::clearDescriptors()
{
    vkResetDescriptorPool(m_device, m_pool, 0);
}

DescriptorAllocator::~DescriptorAllocator()
{
    if(m_pool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(m_device, m_pool, nullptr);
    }
}
