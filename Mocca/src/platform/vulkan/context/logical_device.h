#pragma once

#include <volk.h>

#include <vector>

struct QueueFamilyIndices;

class LogicalDevice
{
public:
    LogicalDevice(
        VkPhysicalDevice physicalDevice,
        const QueueFamilyIndices& indices,
        const std::vector<const char*>& deviceExtensions
    );
    ~LogicalDevice();
    LogicalDevice(const LogicalDevice&) = delete;
    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice(LogicalDevice&&) = delete;
    LogicalDevice& operator=(LogicalDevice&&) = delete;

    VkDevice getHandle() const
    {
        return m_device;
    }
    VkQueue getGraphicsQueue() const
    {
        return m_graphicsQueue;
    }
    VkQueue getPresentQueue() const
    {
        return m_presentQueue;
    }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    // queues are implicitly cleaned
    VkQueue m_graphicsQueue{VK_NULL_HANDLE};
    VkQueue m_presentQueue{VK_NULL_HANDLE};
};