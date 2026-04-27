#include "engine/vk/logical_device.h"
#include "engine/vk/physical_device.h"
#include "engine/vk/utils.h"

#include <set>
#include <vector>

LogicalDevice::LogicalDevice(const PhysicalDevice& physicalDevice)
{
    PhysicalDevice::QueueFamilyIndices indices = physicalDevice.getQueueFamilyIndices();

    std::vector<const char *> deviceExtensions = physicalDevice.getDeviceExtensions();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;

    for(uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{
        //what features we need

    };

    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures,
    };

    VK_CHECK(vkCreateDevice(physicalDevice.getPhysicalDeviceHandle(), &createInfo, nullptr, &m_device));

    volkLoadDevice(m_device);

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);

    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

LogicalDevice::~LogicalDevice()
{
    if(m_device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_device, nullptr);
    }
}