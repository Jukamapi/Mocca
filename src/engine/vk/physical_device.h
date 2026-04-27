#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <volk.h>


class Context;

class PhysicalDevice
{
public:
    PhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    ~PhysicalDevice();
    PhysicalDevice(PhysicalDevice&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&) = delete;
    PhysicalDevice(PhysicalDevice&&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&&) = delete;

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    VkPhysicalDevice getPhysicalDeviceHandle() const { return m_physicalDevice; }
    QueueFamilyIndices getQueueFamilyIndices() const { return m_familyIndices; }
    const std::vector<const char*> getDeviceExtensions() const { return m_deviceExtensions; }

private:
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    int rateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR instance);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR);

    VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
    QueueFamilyIndices m_familyIndices;
    const std::vector<const char*> m_deviceExtensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};