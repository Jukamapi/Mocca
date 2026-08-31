#include "gpu_alloc.h"

#include "core/vk_check.h"


GpuAlloc::GpuAlloc(VkPhysicalDevice physicalDevice, VkDevice device, VkInstance instance)
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

GpuAlloc::~GpuAlloc()
{
    vmaDestroyAllocator(m_allocator);
}