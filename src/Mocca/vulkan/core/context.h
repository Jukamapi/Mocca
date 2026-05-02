#pragma once

#include "Mocca/vulkan/core/instance.h"
#include "Mocca/vulkan/core/logical_device.h"
#include "Mocca/vulkan/core/physical_device.h"
#include "Mocca/vulkan/core/surface.h"


#include <volk.h>

class Window;

class Context
{
public:
    Context(Window& window);
    ~Context();
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;


    VkInstance getInstanceHandle() const
    {
        return m_instance.getHandle();
    }
    VkDevice getDeviceHandle() const
    {
        return m_logicalDevice.getHandle();
    }
    VkPhysicalDevice getPhysicalDeviceHandle() const
    {
        return m_physicalDevice.getHandle();
    }
    VkSurfaceKHR getSurfaceHandle() const
    {
        return m_surface.getHandle();
    }
    const PhysicalDevice& getPhysicalDevice() const
    {
        return m_physicalDevice;
    }
    const LogicalDevice& getLogicalDevice() const
    {
        return m_logicalDevice;
    }

private:
    Instance m_instance;
    Surface m_surface;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
};