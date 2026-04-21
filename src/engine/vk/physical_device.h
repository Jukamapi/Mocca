#pragma once

#include <vulkan/vulkan_core.h>

class Context;

class PhysicalDevice
{
public:
    PhysicalDevice(VkInstance instance);
    ~PhysicalDevice();
    PhysicalDevice(PhysicalDevice&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&) = delete;
    PhysicalDevice(PhysicalDevice&&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&&) = delete;

    bool isDeviceSuitable(VkPhysicalDevice device);

private:
    VkPhysicalDevice m_physicalDevice{};
};