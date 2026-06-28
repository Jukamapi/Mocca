#pragma once
#include <volk.h>
#include <span>

class DescriptorLayout
{
public:
    // needed for vulkan
    static VkDescriptorSetLayoutBinding binding(
        uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t count = 1
    );

    DescriptorLayout() = default;
    DescriptorLayout(
        VkDevice device,
        std::span<const VkDescriptorSetLayoutBinding> bindings,
        void* pNext = nullptr,
        VkDescriptorSetLayoutCreateFlags flags = 0
    );

    ~DescriptorLayout();

    DescriptorLayout(const DescriptorLayout&) = delete;
    DescriptorLayout& operator=(const DescriptorLayout&) = delete;
    DescriptorLayout(DescriptorLayout&& other) noexcept;
    DescriptorLayout& operator=(DescriptorLayout&& other) noexcept;

    VkDescriptorSetLayout getHandle() const
    {
        return m_layout;
    }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkDescriptorSetLayout m_layout{VK_NULL_HANDLE};
};