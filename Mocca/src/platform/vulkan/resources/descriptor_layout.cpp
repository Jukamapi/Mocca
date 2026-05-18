#include "descriptor_layout.h"

#include "platform/vulkan/vk_check.h"

VkDescriptorSetLayoutBinding DescriptorLayout::binding(
    uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t count
)
{
    VkDescriptorSetLayoutBinding newbind{
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = count,
        .stageFlags = stageFlags,
    };

    return newbind;
}

DescriptorLayout::DescriptorLayout(
    VkDevice device,
    std::span<const VkDescriptorSetLayoutBinding> bindings,
    void* pNext,
    VkDescriptorSetLayoutCreateFlags flags
)
{
    m_device = device;

    VkDescriptorSetLayoutCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = pNext,
        .flags = flags,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };

    VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &m_layout));
}

DescriptorLayout::DescriptorLayout(DescriptorLayout&& other) noexcept
    : m_device(other.m_device), m_layout(other.m_layout)
{
    other.m_device = VK_NULL_HANDLE;
    other.m_layout = VK_NULL_HANDLE;
}

DescriptorLayout& DescriptorLayout::operator=(DescriptorLayout&& other) noexcept
{
    if(this != &other)
    {
        if(m_layout != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);

        m_device = other.m_device;
        m_layout = other.m_layout;

        other.m_device = VK_NULL_HANDLE;
        other.m_layout = VK_NULL_HANDLE;
    }

    return *this;
}

DescriptorLayout::~DescriptorLayout()
{
    if(m_layout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
    }
}
