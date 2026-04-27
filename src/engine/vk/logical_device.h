#pragma once

#include "physical_device.h"
#include <volk.h>

class LogicalDevice
{
public:
    LogicalDevice(const PhysicalDevice&);
    ~LogicalDevice();
    LogicalDevice(const LogicalDevice&) = delete;
    LogicalDevice& operator=(const LogicalDevice&) = delete;
    LogicalDevice(LogicalDevice&& ) = delete;
    LogicalDevice& operator=(LogicalDevice&&) = delete;

private:
    VkDevice m_device{ VK_NULL_HANDLE };
    //queues are implicitly cleaned
    VkQueue m_graphicsQueue{ VK_NULL_HANDLE };
    VkQueue m_presentQueue{ VK_NULL_HANDLE }
;};