#include "physical_device.h"

#include <cstdint>
#include <map>
#include <stdexcept>


#include <volk.h>
#include <utility>
#include <vector>


PhysicalDevice::PhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if(deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;

    for(const auto& device : devices)
    {
        int score = rateDeviceSuitability(device, surface);
        candidates.insert(std::make_pair(score, device));
    }

    if(candidates.rbegin()->first > 0)
    {
        m_physicalDevice = candidates.rbegin()->second;
        m_familyIndices = findQueueFamilies(m_physicalDevice, surface);
    }
    else
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

int PhysicalDevice::rateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    if(!deviceFeatures.geometryShader)
    {
        return 0;
    }

    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    if(!indices.isComplete())
    {
        return 0;
    }

    if(!checkDeviceExtensionSupport(device))
    {
        return 0;
    }

    // check after extensions, not before
    SwapchainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
    if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
    {
        return 0;
    }

    return score;
}

bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) const
{

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());


    for(const char* extensionName : m_deviceExtensions)
    {
        bool extensionFound = false;

        for(const auto& extensionProperties : availableExtensions)
        {
            if(strcmp(extensionName, extensionProperties.extensionName) == 0)
            {
                extensionFound = true;
                break;
            }
        }

        if(!extensionFound)
            return false;
    }
    return true;
}

QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


    int i = 0;
    for(const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport)
        {
            indices.presentFamily = i;
        }

        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if(indices.isComplete())
        {
            break;
        }
        i++;
    }

    return indices;
}

SwapchainSupportDetails PhysicalDevice::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if(formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if(presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
