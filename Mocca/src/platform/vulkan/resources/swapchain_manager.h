#pragma once

#include "core/types.h"

#include <memory>

#include <volk.h>

class Swapchain;
class PhysicalDevice;
class LogicalDevice;
class Surface;

class SwapchainManager
{
public:
    SwapchainManager(
        const PhysicalDevice& physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, Extent initialExtent
    );

    ResizeResult handleResize(Extent newExtent);
    void recreate(Extent newExtent);

    Swapchain& getSwapchain()
    {
        return *m_swapchain;
    }

    void markDirty()
    {
        m_isDirty = true;
    }

    bool isDirty() const
    {
        return m_isDirty;
    }


private:
    const PhysicalDevice& m_physicalDevice;
    VkDevice m_logicalDevice;
    VkSurfaceKHR m_surface;

    Extent m_currentExtent;
    std::unique_ptr<Swapchain> m_swapchain;
    bool m_isDirty{false};
};