#pragma once

#include "core/types.h"

#include <memory>

#include <volk.h>

class Swapchain;
class PhysicalDevice;
class LogicalDevice;
class Surface;

// TODO: move this out of resources/ and maybe into its own presentation/

class SwapchainManager
{
public:
    SwapchainManager(
        const PhysicalDevice& physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, Extent initialExtent
    );

    ResizeResult handleResize(Extent newExtent);
    bool recreate(Extent newExtent);

    Swapchain& getSwapchain()
    {
        return *m_swapchain;
    }

    const Extent& getExtent() const
    {
        return m_currentExtent;
    }

    // if swapchain is marked dirty it means it's scheduled for recreating
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