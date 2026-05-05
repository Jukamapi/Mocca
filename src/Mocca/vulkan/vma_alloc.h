#pragma once

#include "Mocca/vulkan/vk_check.h"

#include <vma/vk_mem_alloc.h>

class VmaAlloc
{
public:
    VmaAlloc(VkPhysicalDevice physicalDevice, VkDevice device, VkInstance instance)
    {

        VmaVulkanFunctions vulkanFunctions{};

        // VmaVulkanFunctions::vkGetInstanceProcAddr and vkGetDeviceProcAddr
        VmaAllocatorCreateInfo allocatorInfo{
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice = physicalDevice,
            .device = device,
            .instance = instance,
        };

        VK_CHECK(vmaImportVulkanFunctionsFromVolk(&allocatorInfo, &vulkanFunctions));

        allocatorInfo.pVulkanFunctions = &vulkanFunctions;

        VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator));
    }
    ~VmaAlloc()
    {
        vmaDestroyAllocator(m_allocator);
    }

    VmaAlloc(const VmaAlloc&) = delete;
    VmaAlloc& operator=(const VmaAlloc&) = delete;
    VmaAlloc(VmaAlloc&&) = delete;
    VmaAlloc& operator=(VmaAlloc&&) = delete;

private:
    VmaAllocator m_allocator;
};